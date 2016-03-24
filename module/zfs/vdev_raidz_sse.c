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
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,2,0)
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

#if defined(__x86_64__)
#define	MAKE_CST32_SSE			\
asm volatile("movd %[cast], %%xmm7\n" \
				"pshufd $0, %%xmm7, %%xmm7\n" \
			: \
			: [cast] "r" (0x1d1d1d1d));

#define	LOAD8_SRC_SSE(SRC)						\
asm volatile("movdqa (%[src]), %%xmm0\n" \
				"movdqa 16(%[src]), %%xmm4\n" \
				"movdqa 32(%[src]), %%xmm8\n" \
				"movdqa 48(%[src]), %%xmm12\n" \
		: \
		: [src] "r" (SRC));

#define	COMPUTE8_P_SSE(P)						\
asm volatile("movdqa (%[p]), %%xmm1\n" \
				"movdqa 16(%[p]), %%xmm5\n" \
				"movdqa 32(%[p]), %%xmm9\n" \
				"movdqa 48(%[p]), %%xmm13\n" \
				"pxor %%xmm0,%%xmm1\n" \
				"pxor %%xmm4,%%xmm5\n" \
				"pxor %%xmm8,%%xmm9\n" \
				"pxor %%xmm12,%%xmm13\n" \
				"movdqa %%xmm1, (%[p])\n" \
				"movdqa %%xmm5, 16(%[p])\n" \
				"movdqa %%xmm9, 32(%[p])\n" \
				"movdqa %%xmm13, 48(%[p])\n" \
		: \
		: [p] "r" (P));

#define	COMPUTE8_Q_SSE(Q) \
asm volatile("movdqa (%[q]), %%xmm1\n" \
				"movdqa 16(%[q]), %%xmm5\n" \
				"movdqa 32(%[q]), %%xmm9\n" \
				"movdqa 48(%[q]), %%xmm13\n" \
				"pxor %%xmm2, %%xmm2\n" \
				"pxor %%xmm6, %%xmm6\n" \
				"pxor %%xmm10, %%xmm10\n" \
				"pxor %%xmm14, %%xmm14\n" \
				"pcmpgtb %%xmm1, %%xmm2\n" \
				"pcmpgtb %%xmm5, %%xmm6\n" \
				"pcmpgtb %%xmm9, %%xmm10\n" \
				"pcmpgtb %%xmm13, %%xmm14\n" \
				"paddb %%xmm1,%%xmm1\n" \
				"paddb %%xmm5,%%xmm5\n" \
				"paddb %%xmm9,%%xmm9\n" \
				"paddb %%xmm13,%%xmm13\n" \
				"pand %%xmm7,%%xmm2\n" \
				"pand %%xmm7,%%xmm6\n" \
				"pand %%xmm7,%%xmm10\n" \
				"pand %%xmm7,%%xmm14\n" \
				"pxor %%xmm2, %%xmm1\n" \
				"pxor %%xmm6, %%xmm5\n" \
				"pxor %%xmm10, %%xmm9\n" \
				"pxor %%xmm14, %%xmm13\n" \
				"pxor %%xmm0,%%xmm1\n" \
				"pxor %%xmm4,%%xmm5\n" \
				"pxor %%xmm8,%%xmm9\n" \
				"pxor %%xmm12,%%xmm13\n" \
				"movdqa %%xmm1, (%[q])\n" \
				"movdqa %%xmm5, 16(%[q])\n" \
				"movdqa %%xmm9, 32(%[q])\n" \
				"movdqa %%xmm13, 48(%[q])\n" \
		: \
		: [q] "r" (Q));

#define	COMPUTE8_R_SSE(R) \
asm volatile("movdqa (%[r]), %%xmm1\n" \
				"movdqa 16(%[r]), %%xmm5\n" \
				"movdqa 32(%[r]), %%xmm9\n" \
				"movdqa 48(%[r]), %%xmm13\n" \
				"pxor %%xmm2, %%xmm2\n" \
				"pxor %%xmm6, %%xmm6\n" \
				"pxor %%xmm10, %%xmm10\n" \
				"pxor %%xmm14, %%xmm14\n" \
				"pcmpgtb %%xmm1, %%xmm2\n" \
				"pcmpgtb %%xmm5, %%xmm6\n" \
				"pcmpgtb %%xmm9, %%xmm10\n" \
				"pcmpgtb %%xmm13, %%xmm14\n" \
				"paddb %%xmm1,%%xmm1\n" \
				"paddb %%xmm5,%%xmm5\n" \
				"paddb %%xmm9,%%xmm9\n" \
				"paddb %%xmm13,%%xmm13\n" \
				"pand %%xmm7,%%xmm2\n" \
				"pand %%xmm7,%%xmm6\n" \
				"pand %%xmm7,%%xmm10\n" \
				"pand %%xmm7,%%xmm14\n" \
				"pxor %%xmm2, %%xmm1\n" \
				"pxor %%xmm6, %%xmm5\n" \
				"pxor %%xmm10, %%xmm9\n" \
				"pxor %%xmm14, %%xmm13\n" \
				"pxor %%xmm2, %%xmm2\n" \
				"pxor %%xmm6, %%xmm6\n" \
				"pxor %%xmm10, %%xmm10\n" \
				"pxor %%xmm14, %%xmm14\n" \
				"pcmpgtb %%xmm1, %%xmm2\n" \
				"pcmpgtb %%xmm5, %%xmm6\n" \
				"pcmpgtb %%xmm9, %%xmm10\n" \
				"pcmpgtb %%xmm13, %%xmm14\n" \
				"paddb %%xmm1,%%xmm1\n" \
				"paddb %%xmm5,%%xmm5\n" \
				"paddb %%xmm9,%%xmm9\n" \
				"paddb %%xmm13,%%xmm13\n" \
				"pand %%xmm7,%%xmm2\n" \
				"pand %%xmm7,%%xmm6\n" \
				"pand %%xmm7,%%xmm10\n" \
				"pand %%xmm7,%%xmm14\n" \
				"pxor %%xmm2, %%xmm1\n" \
				"pxor %%xmm6, %%xmm5\n" \
				"pxor %%xmm10, %%xmm9\n" \
				"pxor %%xmm14, %%xmm13\n" \
				"pxor %%xmm0,%%xmm1\n" \
				"pxor %%xmm4,%%xmm5\n" \
				"pxor %%xmm8,%%xmm9\n" \
				"pxor %%xmm12,%%xmm13\n" \
				"movdqa %%xmm1, (%[r])\n" \
				"movdqa %%xmm5, 16(%[r])\n" \
				"movdqa %%xmm9, 32(%[r])\n" \
				"movdqa %%xmm13, 48(%[r])\n" \
		: \
		: [r] "r" (R));

static int raidz_parity_have_sse(void) {
	return (1);
}

int
vdev_raidz_p_sse(void *pbuf, void *sbuf, uint64_t psize, uint64_t csize, void *private)
{
	uint64_t *p = pbuf;
	const uint64_t *src = sbuf;
	int i, ccnt;

	ASSERT(psize >= csize);
	ccnt = csize / sizeof (src[0]);
    kfpu_begin();
	for (i = 0; i < ccnt-7; i += 8, src += 8, p += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_P_SSE(p);
	}
    kfpu_end();
	for (; i < ccnt; i++, src++, p++)
		*p ^= *src;
	return (0);
}
const struct raidz_parity_calls raidz_p_sse = {
	vdev_raidz_p_sse,
	raidz_parity_have_sse,
	"p_sse"
};

int
vdev_raidz_q_sse(void *qbuf, void *sbuf, uint64_t qsize, uint64_t csize, void *private)
{
	uint64_t *q = qbuf;
	const uint64_t *src = sbuf;
	uint64_t mask;
	int i, ccnt, qcnt;

	ASSERT(qsize >= csize);
	ccnt = csize / sizeof (src[0]);
	qcnt = qsize / sizeof (src[0]);

	kfpu_begin();
	MAKE_CST32_SSE;
	for (i = 0; i < ccnt-7; i += 8, src += 8, q += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_Q_SSE(q);
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
const struct raidz_parity_calls raidz_q_sse = {
	vdev_raidz_q_sse,
	raidz_parity_have_sse,
	"q_sse"
};

int
vdev_raidz_r_sse(void *rbuf, void *sbuf, uint64_t rsize, uint64_t csize, void *private)
{
	uint64_t *r = rbuf;
	const uint64_t *src = sbuf;
	uint64_t mask;
	int i, ccnt, rcnt;

	ASSERT(rsize >= csize);
	ccnt = csize / sizeof (src[0]);
	rcnt = rsize / sizeof (src[0]);

	kfpu_begin();
	MAKE_CST32_SSE;
	for (i = 0; i < ccnt-7; i += 8, src += 8, r += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_R_SSE(r);
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
const struct raidz_parity_calls raidz_r_sse = {
	vdev_raidz_r_sse,
	raidz_parity_have_sse,
	"r_sse"
};

#endif
