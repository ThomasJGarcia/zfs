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
vdev_raidz_p_sse(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && !pqr->q && !pqr->r);
	kfpu_begin();
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_P_SSE(pqr->p);
	}
	for (; i < cnt; i++, src++, pqr->p++)
		*pqr->p ^= *src;
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz1_sse = {
	vdev_raidz_p_sse,
	raidz_parity_have_sse,
	"p_sse"
};

int
vdev_raidz_pq_sse(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && !pqr->r);
	kfpu_begin();
	MAKE_CST32_SSE;
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8, pqr->q += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_P_SSE(pqr->p);
		COMPUTE8_Q_SSE(pqr->q);
	}
	for (; i < cnt; i++, src++, pqr->p++, pqr->q++) {
		*pqr->p ^= *src;
		VDEV_RAIDZ_64MUL_2(*pqr->q, mask);
		*pqr->q ^= *src;
	}
	kfpu_end();
	return (0);
}
const struct raidz_parity_calls raidz2_sse = {
	vdev_raidz_pq_sse,
	raidz_parity_have_sse,
	"pq_sse"
};

int
vdev_raidz_pqr_sse(const void *buf, uint64_t size, void *private)
{
	struct pqr_struct *pqr = private;
	const uint64_t *src = buf;
	uint64_t mask;
	int i, j, cnt = size / sizeof (src[0]);

	ASSERT(pqr->p && pqr->q && pqr->r);
	kfpu_begin();
	MAKE_CST32_SSE;
	i = 0;
	for (; i < cnt-7; i += 8, src += 8, pqr->p += 8,
				pqr->q += 8, pqr->r += 8) {
		LOAD8_SRC_SSE(src);
		COMPUTE8_P_SSE(pqr->p);
		COMPUTE8_Q_SSE(pqr->q);
		COMPUTE8_R_SSE(pqr->r);
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
const struct raidz_parity_calls raidz3_sse = {
	vdev_raidz_pqr_sse,
	raidz_parity_have_sse,
	"pqr_sse"
};


#endif
