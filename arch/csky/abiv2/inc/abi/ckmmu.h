// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.
#ifndef __ASM_CSKY_CKMMUV2_H
#define __ASM_CSKY_CKMMUV2_H
#include <abi/reg_ops.h>

static inline void select_mmu_cp(void){}

static inline int  read_mmu_index(void)
{
	return mfcr("cr<0, 15>");
}

static inline void  write_mmu_index(int value)
{
	mtcr("cr<0, 15>", value);
}

static inline int  read_mmu_entrylo0(void)
{
	return mfcr("cr<2, 15>");
}

static inline int  read_mmu_entrylo1(void)
{
	return mfcr("cr<3, 15>");
}

static inline void  write_mmu_pagemask(int value)
{
	mtcr("cr<6, 15>", value);
}

static inline int  read_mmu_entryhi(void)
{
	return mfcr("cr<4, 15>");
}

static inline void  write_mmu_entryhi(int value)
{
	mtcr("cr<4, 15>", value);
}

/*
 * TLB operations.
 */
static inline void tlb_probe(void)
{
	mtcr("cr<8, 15>", 0x80000000);
}

static inline void tlb_read(void)
{
	mtcr("cr<8, 15>", 0x40000000);
}

static inline void tlb_invalid_all(void)
{
#ifdef CONFIG_CPU_HAS_TLBI
	asm volatile("tlbi.all\n");
#else
	mtcr("cr<8, 15>", 0x04000000);
#endif
}

static inline void tlb_invalid_indexed(void)
{
	mtcr("cr<8, 15>", 0x02000000);
}

/* setup hardrefil pgd */
static inline unsigned long get_pgd(void)
{
	return mfcr("cr<29, 15>");
}

static inline void setup_pgd(unsigned long pgd)
{
	mtcr("cr<29, 15>", pgd);
}

static inline void setup_pgd_kernel(unsigned long pgd)
{
	mtcr("cr<28, 15>", pgd);
}

#endif /* __ASM_CSKY_CKMMUV2_H */

