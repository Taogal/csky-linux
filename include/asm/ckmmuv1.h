#ifndef __ASM_CSKY_CKMMUV1_H
#define __ASM_CSKY_CKMMUV1_H

#define CSKY_TLB_SIZE 128

static inline void select_mmu_cp(void)
{
	 __asm__ __volatile__("cpseti cp15\n\t");

}

static inline int read_mmu_index(void)
{
	int __res;
	__asm__ __volatile__(
			"cprcr %0, cpcr0\n\t"
			:"=b" (__res));
	return   __res;
}

static inline void write_mmu_index(int value)
{

	__asm__ __volatile__(
			"cpwcr %0, cpcr0\n\t"
			::"b"(value));
}

static inline int read_mmu_entrylo0(void)
{
	int __res;
	__asm__ __volatile__(
			"cprcr %0, cpcr2\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_entrylo0(int value)
{
	__asm__ __volatile__(
			"cpwcr %0, cpcr2\n\t"
			::"b"(value));
}


static inline int read_mmu_entrylo1(void)
{
	int __res;
	__asm__ __volatile__(
			"cprcr %0, cpcr3\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_entrylo1(int value)
{
	__asm__ __volatile__(
			"cpwcr %0, cpcr3\n\t"
			::"b"(value));
}


static inline int read_mmu_context(void)
{
	int __res;
	__asm__ __volatile__(
			"cprcr %0, cpcr5\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_context(int value)
{
	__asm__ __volatile__(
			"cpwcr %0, cpcr5\n\t"
			::"b"(value));
}


static inline int read_mmu_pagemask(void)
{
	int __res;

	__asm__ __volatile__(
			"cprcr %0, cpcr6\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_pagemask(int value)
{
	__asm__ __volatile__(
			"cpwcr %0, cpcr6\n\t"
			::"b"(value));
}

static inline int read_mmu_wired(void)
{
	int __res;

	__asm__ __volatile__(
			"cprcr %0, cpcr7\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_wired(int value)
{
	__asm__ __volatile__(
			"cpwcr %0, cpcr7\n\t"
			::"b"(value));
}

static inline int read_mmu_entryhi(void)
{
	int __res;
	__asm__ __volatile__(
			"cprcr %0, cpcr4\n\t"
			:"=b" (__res));

	return __res;
}

static inline void write_mmu_entryhi(int value)
{

	__asm__ __volatile__(
			"cpwcr %0, cpcr4\n\t"
			::"b"(value));
}

/*
 * TLB operations.
 */
static inline void tlb_probe(void)
{
	int value = 0x80000000;

	__asm__ __volatile__(
			"cpwcr %0, cpcr8\n\t"
			::"b"(value));
}

static inline void tlb_read(void)
{
	int value = 0x40000000;

	__asm__ __volatile__(
			"cpwcr %0, cpcr8\n\t"
			::"b"(value));
}

static inline void tlb_write_indexed(void)
{
	int value = 0x20000000;

	__asm__ __volatile__(
			"cpwcr %0, cpcr8\n\t"
			::"b"(value));
}

static inline void tlb_write_random(void)
{
	int value = 0x10000000;

	__asm__ __volatile__(
			"cpwcr %0, cpcr8\n\t"
			::"b"(value));
}

static inline void tlb_invalid_indexed(void)
{
	int value = 0x02000000;

	__asm__ __volatile__(
			"cpwcr %0, cpcr8\n\t"
			::"b"(value));
}

#endif /* __ASM_CSKY_CKMMUV1_H */

