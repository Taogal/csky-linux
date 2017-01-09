#include <linux/export.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/io.h>

#include <asm/pgtable.h>

void __iomem *ioremap(phys_addr_t addr, size_t size)
{
	phys_addr_t last_addr;
	unsigned long offset, vaddr;
	struct vm_struct *area;
	pgprot_t prot;

	last_addr = addr + size - 1;
	if (!size || last_addr < addr) {
		return NULL;
	}

	offset = addr & (~PAGE_MASK);
	addr &= PAGE_MASK;
	size = PAGE_ALIGN(size + offset);

	area = get_vm_area_caller(size, VM_IOREMAP, __builtin_return_address(0));
	if (!area) {
		return NULL;
	}
	vaddr = (unsigned long)area->addr;

	prot = __pgprot(_PAGE_PRESENT | __READABLE | __WRITEABLE |
			_PAGE_GLOBAL | _CACHE_UNCACHED);

	if (ioremap_page_range(vaddr, vaddr + size, addr, prot)) {
		free_vm_area(area);
		return NULL;
	}

	return (void __iomem *)(vaddr + offset);
}
EXPORT_SYMBOL(ioremap);

void iounmap(void __iomem *addr)
{
	vunmap((void *)((unsigned long)addr & PAGE_MASK));
}
EXPORT_SYMBOL(iounmap);

