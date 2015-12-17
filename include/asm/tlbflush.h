/*
 * linux/arch/csky/include/asm/tlbflush.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 C-SKY Microsystems co.,ltd.  All rights reserved.
 * Copyright (C) 2006 by Li Chunqiang (chunqiang_li@c-sky.com)
 * Copyright (C) 2009 by Ye yun (yun_ye@c-sky.com)
 * Copyright (C) 2011 by Dou Shaobin (shaobin_dou@c-sky.com)
 *
 */

#ifndef __ASM_TLBFLUSH_H
#define __ASM_TLBFLUSH_H
#ifdef CONFIG_MMU
/*
 * TLB flushing:
 *
 *  - flush_tlb_all() flushes all processes TLB entries
 *  - flush_tlb_mm(mm) flushes the specified mm context TLB entries
 *  - flush_tlb_page(vma, vmaddr) flushes one page
 *  - flush_tlb_range(vma, start, end) flushes a range of pages
 *  - flush_tlb_kernel_range(start, end) flushes a range of kernel pages
 */
extern void local_flush_tlb_all(void);
extern void local_flush_tlb_mm(struct mm_struct *mm);
extern void local_flush_tlb_range(struct vm_area_struct *vma,
        unsigned long start, unsigned long end);
extern void local_flush_tlb_kernel_range(unsigned long start,
        unsigned long end);
extern void local_flush_tlb_page(struct vm_area_struct *vma,
        unsigned long page);
extern void local_flush_tlb_one(unsigned long vaddr);

#ifdef CONFIG_SMP

extern void flush_tlb_all(void);
extern void flush_tlb_mm(struct mm_struct *);
extern void flush_tlb_range(struct vm_area_struct *vma, unsigned long,
        unsigned long);
extern void flush_tlb_kernel_range(unsigned long, unsigned long);
extern void flush_tlb_page(struct vm_area_struct *, unsigned long);
extern void flush_tlb_one(unsigned long vaddr);

#else /* CONFIG_SMP */

#define flush_tlb_all()                 local_flush_tlb_all()
#define flush_tlb_mm(mm)                local_flush_tlb_mm(mm)
#define flush_tlb_range(vma, vmaddr, end)   \
        local_flush_tlb_range(vma, vmaddr, end)
#define flush_tlb_kernel_range(vmaddr,end) \
        local_flush_tlb_kernel_range(vmaddr, end)
#define flush_tlb_page(vma, page)       local_flush_tlb_page(vma, page)
#define flush_tlb_one(vaddr)            local_flush_tlb_one(vaddr)

#endif /* CONFIG_SMP */

#endif /*CONFIG_MMU*/
#endif
