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

#if defined(__x86_64__) && defined(_KERNEL) && defined(CONFIG_AS_AVX)
#define	MAKE_CST32_AVX128			\
asm volatile("vmovd %[cast], %%xmm7\n" \
				"vpshufd $0, %%xmm7, %%xmm7\n" \
			: \
			: [cast] "r" (0x1d1d1d1d));

#define	LOAD8_SRC_AVX128(SRC)						\
asm volatile("vmovdqa (%[src]), %%xmm0\n" \
				"vmovdqa 16(%[src]), %%xmm4\n" \
				"vmovdqa 32(%[src]), %%xmm8\n" \
				"vmovdqa 48(%[src]), %%xmm12\n" \
		: \
		: [src] "r" (SRC));

#define	COMPUTE8_P_AVX128(P)					\
asm volatile("vmovdqa (%[p]), %%xmm1\n" \
				"vmovdqa 16(%[p]), %%xmm5\n" \
				"vmovdqa 32(%[p]), %%xmm9\n" \
				"vmovdqa 48(%[p]), %%xmm13\n" \
				"vpxor %%xmm0, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm4, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm8, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm12, %%xmm13, %%xmm13\n" \
				"vmovdqa %%xmm1, (%[p])\n" \
				"vmovdqa %%xmm5, 16(%[p])\n" \
				"vmovdqa %%xmm9, 32(%[p])\n" \
				"vmovdqa %%xmm13, 48(%[p])\n" \
		: \
		: [p] "r" (P));

#define	COMPUTE8_Q_AVX128(Q)						\
asm volatile("vmovdqa (%[q]), %%xmm1\n" \
				"vmovdqa 16(%[q]), %%xmm5\n" \
				"vmovdqa 32(%[q]), %%xmm9\n" \
				"vmovdqa 48(%[q]), %%xmm13\n" \
				"vpxor %%xmm14, %%xmm14, %%xmm14\n" \
				"vpcmpgtb %%xmm1, %%xmm14, %%xmm2\n" \
				"vpcmpgtb %%xmm5, %%xmm14, %%xmm6\n" \
				"vpcmpgtb %%xmm9, %%xmm14, %%xmm10\n" \
				"vpcmpgtb %%xmm13, %%xmm14, %%xmm14\n" \
				"vpaddb %%xmm1, %%xmm1, %%xmm1\n" \
				"vpaddb %%xmm5, %%xmm5, %%xmm5\n" \
				"vpaddb %%xmm9, %%xmm9, %%xmm9\n" \
				"vpaddb %%xmm13, %%xmm13, %%xmm13\n" \
				"vpand %%xmm7, %%xmm2, %%xmm2\n" \
				"vpand %%xmm7, %%xmm6, %%xmm6\n" \
				"vpand %%xmm7, %%xmm10, %%xmm10\n" \
				"vpand %%xmm7, %%xmm14, %%xmm14\n" \
				"vpxor %%xmm2, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm6, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm10, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm14, %%xmm13, %%xmm13\n" \
				"vpxor %%xmm0, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm4, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm8, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm12, %%xmm13, %%xmm13\n" \
				"vmovdqa %%xmm1, (%[q])\n" \
				"vmovdqa %%xmm5, 16(%[q])\n" \
				"vmovdqa %%xmm9, 32(%[q])\n" \
				"vmovdqa %%xmm13, 48(%[q])\n" \
		: \
		: [q] "r" (Q));

#define	COMPUTE8_R_AVX128(R)						\
asm volatile("vmovdqa (%[r]), %%xmm1\n" \
				"vmovdqa 16(%[r]), %%xmm5\n" \
				"vmovdqa 32(%[r]), %%xmm9\n" \
				"vmovdqa 48(%[r]), %%xmm13\n" \
				"vpxor %%xmm14, %%xmm14, %%xmm14\n" \
				"vpcmpgtb %%xmm1, %%xmm14, %%xmm2\n" \
				"vpcmpgtb %%xmm5, %%xmm14, %%xmm6\n" \
				"vpcmpgtb %%xmm9, %%xmm14, %%xmm10\n" \
				"vpcmpgtb %%xmm13, %%xmm14, %%xmm14\n" \
				"vpaddb %%xmm1, %%xmm1, %%xmm1\n" \
				"vpaddb %%xmm5, %%xmm5, %%xmm5\n" \
				"vpaddb %%xmm9, %%xmm9, %%xmm9\n" \
				"vpaddb %%xmm13, %%xmm13, %%xmm13\n" \
				"vpand %%xmm7, %%xmm2, %%xmm2\n" \
				"vpand %%xmm7, %%xmm6, %%xmm6\n" \
				"vpand %%xmm7, %%xmm10, %%xmm10\n" \
				"vpand %%xmm7, %%xmm14, %%xmm14\n" \
				"vpxor %%xmm2, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm6, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm10, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm14, %%xmm13, %%xmm13\n" \
				"vpxor %%xmm14, %%xmm14, %%xmm14\n" \
				"vpcmpgtb %%xmm1, %%xmm14, %%xmm2\n" \
				"vpcmpgtb %%xmm5, %%xmm14, %%xmm6\n" \
				"vpcmpgtb %%xmm9, %%xmm14, %%xmm10\n" \
				"vpcmpgtb %%xmm13, %%xmm14, %%xmm14\n" \
				"vpaddb %%xmm1, %%xmm1, %%xmm1\n" \
				"vpaddb %%xmm5, %%xmm5, %%xmm5\n" \
				"vpaddb %%xmm9, %%xmm9, %%xmm9\n" \
				"vpaddb %%xmm13, %%xmm13, %%xmm13\n" \
				"vpand %%xmm7, %%xmm2, %%xmm2\n" \
				"vpand %%xmm7, %%xmm6, %%xmm6\n" \
				"vpand %%xmm7, %%xmm10, %%xmm10\n" \
				"vpand %%xmm7, %%xmm14, %%xmm14\n" \
				"vpxor %%xmm2, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm6, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm10, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm14, %%xmm13, %%xmm13\n" \
				"vpxor %%xmm0, %%xmm1, %%xmm1\n" \
				"vpxor %%xmm4, %%xmm5, %%xmm5\n" \
				"vpxor %%xmm8, %%xmm9, %%xmm9\n" \
				"vpxor %%xmm12, %%xmm13, %%xmm13\n" \
				"vmovdqa %%xmm1, (%[r])\n" \
				"vmovdqa %%xmm5, 16(%[r])\n" \
				"vmovdqa %%xmm9, 32(%[r])\n" \
				"vmovdqa %%xmm13, 48(%[r])\n" \
		: \
		: [r] "r" (R));

static int raidz_parity_have_avx128(void) {
	return (boot_cpu_has(X86_FEATURE_AVX));
}

int
vdev_raidz_p_avx128(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && !pqr->q && !pqr->r);
	kfpu_begin();
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8) {
		LOAD8_SRC_AVX128(src);
		COMPUTE8_P_AVX128(pqr->p);
	}
	for (; i < cnt; i++, src++, pqr->p++)
		*pqr->p ^= *src;
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz1_avx128 = {
	vdev_raidz_p_avx128,
	raidz_parity_have_avx128,
	"p_avx128"
};

int
vdev_raidz_pq_avx128(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && !pqr->r);
	kfpu_begin();
	MAKE_CST32_AVX128;
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8, pqr->q += 8) {
		LOAD8_SRC_AVX128(src);
		COMPUTE8_P_AVX128(pqr->p);
		COMPUTE8_Q_AVX128(pqr->q);
	}
	for (; i < cnt; i++, src++, pqr->p++, pqr->q++) {
		*pqr->p ^= *src;
		VDEV_RAIDZ_64MUL_2(*pqr->q, mask);
		*pqr->q ^= *src;
	}
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz2_avx128 = {
	vdev_raidz_pq_avx128,
	raidz_parity_have_avx128,
	"pq_avx128"
};

int
vdev_raidz_pqr_avx128(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && pqr->r);
	kfpu_begin();
	MAKE_CST32_AVX128;
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8,
				pqr->q += 8, pqr->r += 8) {
		LOAD8_SRC_AVX128(src);
		COMPUTE8_P_AVX128(pqr->p);
		COMPUTE8_Q_AVX128(pqr->q);
		COMPUTE8_R_AVX128(pqr->r);
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
const struct raidz_parity_calls raidz3_avx128 = {
	vdev_raidz_pqr_avx128,
	raidz_parity_have_avx128,
	"pqr_avx128"
};


#endif
