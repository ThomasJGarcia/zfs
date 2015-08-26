/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2012, 2014 by Delphix. All rights reserved.
 * Copyright (c) 2015 AtoS <romain.dolbeau@atos.net>. All rights reserved.
 */

#if defined(_KERNEL) && defined(__x86_64__)
#include <asm/i387.h>
#include <asm/cpufeature.h>
#endif
#include <sys/zfs_context.h>
#include <sys/vdev_impl.h>
#include <sys/fs/zfs.h>
#include <sys/fm/fs/zfs.h>

#include <sys/vdev_raidz.h>

#if defined(__x86_64__) && defined(_KERNEL) && defined(CONFIG_AS_AVX2)
#define	MAKE_CST32_AVX2(regx, regy, val)		\
	asm volatile("vmovd %0,%%"#regx : : "r"(val));	\
	asm volatile("vpbroadcastd %"#regx",%"#regy)

#define	LOAD16_SRC_AVX2(SRC)						\
    asm volatile("vmovdqa (%[src]), %%ymm0\n" \
                 "vmovdqa 32(%[src]), %%ymm4\n" \
                 "vmovdqa 64(%[src]), %%ymm8\n" \
                 "vmovdqa 96(%[src]), %%ymm12\n" \
            : \
            : [src] "r" (SRC) \
            : "memory");

#define	COMPUTE16_P_AVX2(P)					\
    asm volatile("vmovdqa (%[p]), %%ymm1\n" \
                 "vmovdqa 32(%[p]), %%ymm5\n" \
                 "vmovdqa 64(%[p]), %%ymm9\n" \
                 "vmovdqa 96(%[p]), %%ymm13\n" \
                 "vpxor %%ymm0, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm4, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm8, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm12, %%ymm13, %%ymm13\n" \
                 "vmovdqa %%ymm1, (%[p])\n" \
                 "vmovdqa %%ymm5, 32(%[p])\n" \
                 "vmovdqa %%ymm9, 64(%[p])\n" \
                 "vmovdqa %%ymm13, 96(%[p])\n" \
            : \
            : [p] "r" (P) \
            : "memory");

#define	COMPUTE16_Q_AVX2(Q)						\
    asm volatile("vmovdqa (%[q]), %%ymm1\n" \
                 "vmovdqa 32(%[q]), %%ymm5\n" \
                 "vmovdqa 64(%[q]), %%ymm9\n" \
                 "vmovdqa 96(%[q]), %%ymm13\n" \
                 "vmovd %[cast], %%xmm3\n" \
                 "vpbroadcastd %%xmm3, %%ymm3\n" \
                 "vpxor %%ymm14, %%ymm14, %%ymm14\n" \
                 "vpcmpgtb %%ymm1, %%ymm14, %%ymm2\n" \
                 "vpcmpgtb %%ymm5, %%ymm14, %%ymm6\n" \
                 "vpcmpgtb %%ymm9, %%ymm14, %%ymm10\n" \
                 "vpcmpgtb %%ymm13, %%ymm14, %%ymm14\n" \
                 "vpaddb %%ymm1, %%ymm1, %%ymm1\n" \
                 "vpaddb %%ymm5, %%ymm5, %%ymm5\n" \
                 "vpaddb %%ymm9, %%ymm9, %%ymm9\n" \
                 "vpaddb %%ymm13, %%ymm13, %%ymm13\n" \
                 "vpand %%ymm3, %%ymm2, %%ymm2\n" \
                 "vpand %%ymm3, %%ymm6, %%ymm6\n" \
                 "vpand %%ymm3, %%ymm10, %%ymm10\n" \
                 "vpand %%ymm3, %%ymm14, %%ymm14\n" \
                 "vpxor %%ymm2, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm6, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm10, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm14, %%ymm13, %%ymm13\n" \
                 "vpxor %%ymm0, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm4, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm8, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm12, %%ymm13, %%ymm13\n" \
                 "vmovdqa %%ymm1, (%[q])\n" \
                 "vmovdqa %%ymm5, 32(%[q])\n" \
                 "vmovdqa %%ymm9, 64(%[q])\n" \
                 "vmovdqa %%ymm13, 96(%[q])\n" \
            : \
            : [q] "r" (Q), [cast] "r" (0x1d1d1d1d) \
            : "memory");

#define	COMPUTE16_R_AVX2(R)						\
    asm volatile("vmovdqa (%[r]), %%ymm1\n" \
                 "vmovdqa 32(%[r]), %%ymm5\n" \
                 "vmovdqa 64(%[r]), %%ymm9\n" \
                 "vmovdqa 96(%[r]), %%ymm13\n" \
                 "vmovd %[cast], %%xmm3\n" \
                 "vpbroadcastd %%xmm3, %%ymm3\n" \
                 "vpxor %%ymm14, %%ymm14, %%ymm14\n" \
                 "vpcmpgtb %%ymm1, %%ymm14, %%ymm2\n" \
                 "vpcmpgtb %%ymm5, %%ymm14, %%ymm6\n" \
                 "vpcmpgtb %%ymm9, %%ymm14, %%ymm10\n" \
                 "vpcmpgtb %%ymm13, %%ymm14, %%ymm14\n" \
                 "vpaddb %%ymm1, %%ymm1, %%ymm1\n" \
                 "vpaddb %%ymm5, %%ymm5, %%ymm5\n" \
                 "vpaddb %%ymm9, %%ymm9, %%ymm9\n" \
                 "vpaddb %%ymm13, %%ymm13, %%ymm13\n" \
                 "vpand %%ymm3, %%ymm2, %%ymm2\n" \
                 "vpand %%ymm3, %%ymm6, %%ymm6\n" \
                 "vpand %%ymm3, %%ymm10, %%ymm10\n" \
                 "vpand %%ymm3, %%ymm14, %%ymm14\n" \
                 "vpxor %%ymm2, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm6, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm10, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm14, %%ymm13, %%ymm13\n" \
                 "vpxor %%ymm14, %%ymm14, %%ymm14\n" \
                 "vpcmpgtb %%ymm1, %%ymm14, %%ymm2\n" \
                 "vpcmpgtb %%ymm5, %%ymm14, %%ymm6\n" \
                 "vpcmpgtb %%ymm9, %%ymm14, %%ymm10\n" \
                 "vpcmpgtb %%ymm13, %%ymm14, %%ymm14\n" \
                 "vpaddb %%ymm1, %%ymm1, %%ymm1\n" \
                 "vpaddb %%ymm5, %%ymm5, %%ymm5\n" \
                 "vpaddb %%ymm9, %%ymm9, %%ymm9\n" \
                 "vpaddb %%ymm13, %%ymm13, %%ymm13\n" \
                 "vpand %%ymm3, %%ymm2, %%ymm2\n" \
                 "vpand %%ymm3, %%ymm6, %%ymm6\n" \
                 "vpand %%ymm3, %%ymm10, %%ymm10\n" \
                 "vpand %%ymm3, %%ymm14, %%ymm14\n" \
                 "vpxor %%ymm2, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm6, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm10, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm14, %%ymm13, %%ymm13\n" \
                 "vpxor %%ymm0, %%ymm1, %%ymm1\n" \
                 "vpxor %%ymm4, %%ymm5, %%ymm5\n" \
                 "vpxor %%ymm8, %%ymm9, %%ymm9\n" \
                 "vpxor %%ymm12, %%ymm13, %%ymm13\n" \
                 "vmovdqa %%ymm1, (%[r])\n" \
                 "vmovdqa %%ymm5, 32(%[r])\n" \
                 "vmovdqa %%ymm9, 64(%[r])\n" \
                 "vmovdqa %%ymm13, 96(%[r])\n" \
            : \
            : [r] "r" (R), [cast] "r" (0x1d1d1d1d) \
            : "memory");

static int raidz_parity_have_avx2(void) {
	return (boot_cpu_has(X86_FEATURE_AVX2));
}

int
vdev_raidz_p_avx2(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && !pqr->q && !pqr->r);
	kfpu_begin();
	i = 0;
	for (; i < cnt-15; i += 16, src += 16, pqr->p += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_P_AVX2(pqr->p);
	}
	for (; i < cnt; i++, src++, pqr->p++)
		*pqr->p ^= *src;
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz1_avx2 = {
	vdev_raidz_p_avx2,
	raidz_parity_have_avx2,
	"p_avx2"
};

int
vdev_raidz_pq_avx2(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && !pqr->r);
	kfpu_begin();
	i = 0;
	for (; i < cnt-15; i += 16, src += 16, pqr->p += 16, pqr->q += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_P_AVX2(pqr->p);
		COMPUTE16_Q_AVX2(pqr->q);
	}
	for (; i < cnt; i++, src++, pqr->p++, pqr->q++) {
		*pqr->p ^= *src;
		VDEV_RAIDZ_64MUL_2(*pqr->q, mask);
		*pqr->q ^= *src;
	}
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz2_avx2 = {
	vdev_raidz_pq_avx2,
	raidz_parity_have_avx2,
	"pq_avx2"
};

int
vdev_raidz_pqr_avx2(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && pqr->r);
	kfpu_begin();
	i = 0;
	for (; i < cnt-15; i += 16, src += 16, pqr->p += 16,
				pqr->q += 16, pqr->r += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_P_AVX2(pqr->p);
		COMPUTE16_Q_AVX2(pqr->q);
		COMPUTE16_R_AVX2(pqr->r);
	}
	for (; i < cnt; i++, src++, pqr->p++, pqr->q++, pqr->r++) {
		*pqr->p ^= *src;
		VDEV_RAIDZ_64MUL_2(*pqr->q, mask);
		*pqr->q ^= *src;
		VDEV_RAIDZ_64MUL_4(*pqr->r, mask);
		*pqr->r ^= *src;
	}
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz3_avx2 = {
	vdev_raidz_pqr_avx2,
	raidz_parity_have_avx2,
	"pqr_avx2"
};


#endif
