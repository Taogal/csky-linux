#ifndef __ASM_CSKY_CKMMUV2_H
#define __ASM_CSKY_CKMMUV2_H

#define CSKY_TLB_SIZE 128

static inline int  read_mmu_index(void)
{
	int __res;
	__asm__ __volatile__("mfcr %0,cr<0, 15>\n\t"
					:"=r" (__res));
	return   __res;
}

static inline void  write_mmu_index(int value)
{
	__asm__ __volatile__("mtcr %0,cr<0, 15>\n\t"
					: :"r" (value));
}

static inline int  read_mmu_entrylo0(void)
{
	int __res;
	__asm__ __volatile__("mfcr %0,cr<2, 15>\n\t"
					:"=r" (__res));
	return   __res;
}

static inline void  write_mmu_entrylo0(int value)
{
	__asm__ __volatile__("mtcr %0,cr<2, 15>\n\t"
					: :"r" (value));
}

static inline int  read_mmu_entrylo1(void)
{
	int __res;

	__asm__ __volatile__("mfcr %0,cr<3, 15>\n\t"
					:"=r" (__res));
	return   __res;
}

static inline void  write_mmu_entrylo1(int value)
{
	__asm__ __volatile__("mtcr %0,cr<3, 15>\n\t"
					: :"r" (value));
}

static inline int  read_mmu_pagemask(void)
{
	int __res;

	__asm__ __volatile__("mfcr %0,cr<6, 15>\n\t"
					:"=r" (__res));
	return   __res;
}

static inline void  write_mmu_pagemask(int value)
{
	__asm__ __volatile__("mtcr %0,cr<6, 15>\n\t"
					: :"r" (value));
}

static inline int  read_mmu_entryhi(void)
{
	int __res;

	__asm__ __volatile__("mfcr %0,cr<4, 15>\n\t"
					:"=r" (__res));
	return   __res;
}

static inline void  write_mmu_entryhi(int value)
{
	__asm__ __volatile__("mtcr %0,cr<4, 15>\n\t"
					: :"r" (value));
}

/*
 * TLB operations.
 */
static inline void tlb_probe(void)
{
	int value = 0x80000000;

	__asm__ __volatile__("mtcr %0,cr<8, 15>\n\t"
					: :"r" (value));
}

static inline void tlb_read(void)
{
	int value = 0x40000000;

	__asm__ __volatile__("mtcr %0,cr<8, 15>\n\t"
					: :"r" (value));
}

static inline void tlb_write_indexed(void)
{
	int value = 0x20000000;

	__asm__ __volatile__("mtcr %0,cr<8,15>\n\t"
					: :"r" (value));
}

static inline void tlb_write_random(void)
{
	int value = 0x10000000;

	__asm__ __volatile__("mtcr %0,cr<8, 15>\n\t"
					: :"r" (value));
}

static inline void tlb_invalid_all(void)
{
	int value = 0x04000000;

	__asm__ __volatile__("mtcr %0,cr<8, 15>\n\t"
					: :"r" (value));
}

static inline void tlb_invalid_indexed(void)
{
	int value = 0x02000000;

	__asm__ __volatile__("mtcr %0,cr<8, 15>\n\t"
					: :"r" (value));
}

#endif /* __ASM_CSKY_CKMMUV1_H */

