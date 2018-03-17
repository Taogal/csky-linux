#ifndef __ABI_CSKY_CACHEFLUSH_H
#define __ABI_CSKY_CACHEFLUSH_H

#include <linux/compiler.h>
#include <asm/string.h>
#include <asm/cache.h>

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1
extern void flush_dcache_page(struct page *);

#define flush_cache_mm(mm)		cache_wbinv_all()
#define flush_cache_page(vma,page,pfn)	cache_wbinv_all()
#define flush_cache_dup_mm(mm)		cache_wbinv_all()
#define flush_icache_page(vma, page)	icache_inv_all()

#define flush_cache_range(mm,start,end)	cache_wbinv_range(start, end)
#define flush_cache_vmap(start, end)	cache_wbinv_range(start, end)
#define flush_cache_vunmap(start, end)  cache_wbinv_range(start, end)
#define flush_icache_range(start, end)	icache_inv_range(start, end)

#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
do{ \
	cache_wbinv_all(); \
	memcpy(dst, src, len); \
	icache_inv_all(); \
}while(0)

#define copy_to_user_page(vma, page, vaddr, dst, src, len) \
do{ \
	cache_wbinv_all(); \
	memcpy(dst, src, len); \
}while(0)

#define flush_dcache_mmap_lock(mapping)		do{}while(0)
#define flush_dcache_mmap_unlock(mapping)	do{}while(0)

#endif /* __ABI_CSKY_CACHEFLUSH_H */

