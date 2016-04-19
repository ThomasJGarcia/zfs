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
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 2, 0)
#include <asm/i387.h>
#else
#include <linux/types.h>
#include <asm/fpu/api.h>
#endif
#include <asm/cpufeature.h>
#endif
#include <sys/zfs_context.h>
#include <sys/vdev_impl.h>
#include <sys/fs/zfs.h>
#include <sys/fm/fs/zfs.h>

#include <sys/vdev_raidz.h>

#if defined(__x86_64__) && defined(_KERNEL) && defined(CONFIG_AS_AVX2)
#define	MAKE_CST32_AVX2		\
asm volatile("vmovd %[cast], %%xmm7\n" \
				"vpbroadcastd %%xmm7, %%ymm7\n" \
	: \
	: [cast] "r" (0x1d1d1d1d));

#define	LOAD16_SRC_AVX2(SRC)						\
asm volatile("vmovdqa (%[src]), %%ymm0\n" \
				"vmovdqa 32(%[src]), %%ymm4\n" \
				"vmovdqa 64(%[src]), %%ymm8\n" \
				"vmovdqa 96(%[src]), %%ymm12\n" \
	: \
	: [src] "r" (SRC));

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
	: [p] "r" (P));

#define	COMPUTE16_Q_AVX2(Q)						\
asm volatile("vmovdqa (%[q]), %%ymm1\n" \
				"vmovdqa 32(%[q]), %%ymm5\n" \
				"vmovdqa 64(%[q]), %%ymm9\n" \
				"vmovdqa 96(%[q]), %%ymm13\n" \
				"vpxor %%ymm14, %%ymm14, %%ymm14\n" \
				"vpcmpgtb %%ymm1, %%ymm14, %%ymm2\n" \
				"vpcmpgtb %%ymm5, %%ymm14, %%ymm6\n" \
				"vpcmpgtb %%ymm9, %%ymm14, %%ymm10\n" \
				"vpcmpgtb %%ymm13, %%ymm14, %%ymm14\n" \
				"vpaddb %%ymm1, %%ymm1, %%ymm1\n" \
				"vpaddb %%ymm5, %%ymm5, %%ymm5\n" \
				"vpaddb %%ymm9, %%ymm9, %%ymm9\n" \
				"vpaddb %%ymm13, %%ymm13, %%ymm13\n" \
				"vpand %%ymm7, %%ymm2, %%ymm2\n" \
				"vpand %%ymm7, %%ymm6, %%ymm6\n" \
				"vpand %%ymm7, %%ymm10, %%ymm10\n" \
				"vpand %%ymm7, %%ymm14, %%ymm14\n" \
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
	: [q] "r" (Q));

#define	COMPUTE16_R_AVX2(R)						\
asm volatile("vmovdqa (%[r]), %%ymm1\n" \
				"vmovdqa 32(%[r]), %%ymm5\n" \
				"vmovdqa 64(%[r]), %%ymm9\n" \
				"vmovdqa 96(%[r]), %%ymm13\n" \
				"vpxor %%ymm14, %%ymm14, %%ymm14\n" \
				"vpcmpgtb %%ymm1, %%ymm14, %%ymm2\n" \
				"vpcmpgtb %%ymm5, %%ymm14, %%ymm6\n" \
				"vpcmpgtb %%ymm9, %%ymm14, %%ymm10\n" \
				"vpcmpgtb %%ymm13, %%ymm14, %%ymm14\n" \
				"vpaddb %%ymm1, %%ymm1, %%ymm1\n" \
				"vpaddb %%ymm5, %%ymm5, %%ymm5\n" \
				"vpaddb %%ymm9, %%ymm9, %%ymm9\n" \
				"vpaddb %%ymm13, %%ymm13, %%ymm13\n" \
				"vpand %%ymm7, %%ymm2, %%ymm2\n" \
				"vpand %%ymm7, %%ymm6, %%ymm6\n" \
				"vpand %%ymm7, %%ymm10, %%ymm10\n" \
				"vpand %%ymm7, %%ymm14, %%ymm14\n" \
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
				"vpand %%ymm7, %%ymm2, %%ymm2\n" \
				"vpand %%ymm7, %%ymm6, %%ymm6\n" \
				"vpand %%ymm7, %%ymm10, %%ymm10\n" \
				"vpand %%ymm7, %%ymm14, %%ymm14\n" \
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
	: [r] "r" (R));

static int raidz_parity_have_avx2(void) {
	return (boot_cpu_has(X86_FEATURE_AVX2));
}

int
vdev_raidz_p_avx2(void *pbuf, void *sbuf, uint64_t psize, uint64_t csize, void *private)
{
	uint64_t *p = pbuf;
	const uint64_t *src = sbuf;
	int i, ccnt;

	ASSERT(psize >= csize);
	ccnt = csize / sizeof (src[0]);
	kfpu_begin();
	for (i = 0; i < ccnt-15; i += 16, src += 16, p += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_P_AVX2(p);
	}
	kfpu_end();
	for (; i < ccnt; i++, src++, p++)
		*p ^= *src;
	return (0);
}
const struct raidz_parity_calls raidz_p_avx2 = {
	vdev_raidz_p_avx2,
	raidz_parity_have_avx2,
	"p_avx2"
};

int
vdev_raidz_q_avx2(void *qbuf, void *sbuf, uint64_t qsize, uint64_t csize, void *private)
{
	uint64_t *q = qbuf;
	const uint64_t *src = sbuf;
	uint64_t mask;
	int i, ccnt, qcnt;

	ASSERT(qsize >= csize);
	ccnt = csize / sizeof (src[0]);
	qcnt = qsize / sizeof (src[0]);

	kfpu_begin();
	MAKE_CST32_AVX2;
	for (i = 0; i < ccnt-15; i += 16, src += 16, q += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_Q_AVX2(q);
	}
	kfpu_end();
	for (; i < ccnt; i++, src++, q++) {
		VDEV_RAIDZ_64MUL_2(*q, mask);
		*q ^= *src;
	}
	/*
	 * treat short columns as though they are full of 0s.
	 */
	for (; i < qcnt; i++, q++) {
		VDEV_RAIDZ_64MUL_2(*q, mask);
	}
	return (0);
}
const struct raidz_parity_calls raidz_q_avx2 = {
	vdev_raidz_q_avx2,
	raidz_parity_have_avx2,
	"q_avx2"
};

int
vdev_raidz_r_avx2(void *rbuf, void *sbuf, uint64_t rsize, uint64_t csize, void *private)
{
	uint64_t *r = rbuf;
	const uint64_t *src = sbuf;
	uint64_t mask;
	int i, ccnt, rcnt;

	ASSERT(rsize >= csize);
	ccnt = csize / sizeof (src[0]);
	rcnt = rsize / sizeof (src[0]);

	kfpu_begin();
	MAKE_CST32_AVX2;
	for (i = 0; i < ccnt-15; i += 16, src += 16, r += 16) {
		LOAD16_SRC_AVX2(src);
		COMPUTE16_R_AVX2(r);
	}
	kfpu_end();
	for (; i < ccnt; i++, src++, r++) {
		VDEV_RAIDZ_64MUL_4(*r, mask);
		*r ^= *src;
	}
	/*
	 * treat short columns as though they are full of 0s.
	 */
	for (; i < rcnt; i++, r++) {
		VDEV_RAIDZ_64MUL_4(*r, mask);
	}
	return (0);
}
const struct raidz_parity_calls raidz_r_avx2 = {
	vdev_raidz_r_avx2,
	raidz_parity_have_avx2,
	"r_avx2"
};

#endif
