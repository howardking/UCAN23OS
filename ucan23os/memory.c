
/* filename:    memory.c
 * description: 包含了有内存操作的相关函数
 * author:      Howard
 * date:        2013-12-01
 * version:     v1.0
 */

#include "bootpack.h"

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if (0!=(eflg & EFLAGS_AC_BIT)){
		flg486 = 1;
	}
	eflg &= -EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	
	if (0!=flg486){
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}
	
	i = memtest_sub(start, end);
	
	if (0!=flg486){
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}
	
	return i;
}
/*unsigned int memtest_sub(unsigned int start, unsigned int end)
{
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for (i=start; i<=end; i+=0x1000){
		p = (unsigned int *) (i+0xffc);
		old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if (*p != pat1){
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;
		if (*p != pat0){
			goto not_memory;
		}
		*p = old;
	}
	return i;
}*/

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}
unsigned int memman_total(struct MEMMAN *man){
	unsigned int i, t = 0;
	for (i=0; i<man->frees; i++){
		t += man->free[i].size;
	}
	return t;
}
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, a;
	for (i=0; i<man->frees; i++){
		if (man->free[i].size>=size){
			/*找到了足够大的内存*/
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (0==man->free[i].size){
				man->frees--;
				for (; i<man->frees; i++){
					man->free[i] = man->free[i+1];
				}
			}
			return a;
		}
	}
	return 0; /*没有可用的内存空间（内存分配失败）*/
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i,j;
	/*为便于归纳内存，将free[]按照addr的顺序排列*/
	/*所以先决定放在哪里*/
	for (i=0; i<man->frees; i++){
		if (man->free[i].addr > addr){
			break;
		}
	}
	/* free[i-1].addr<addr< free[i].addr*/
	if (i>0){
		if (man->free[i-1].addr + man->free[i-1].size == addr){
			man->free[i-1].size += size;
			if (i<man->frees){
				if (addr+size == man->free[i+1].addr){
					man->free[i-1].size += man->free[i].size;
					man->frees--;
					for (; i<man->frees; i++){
						man->free[i] = man->free[i+1];
					}
				}
			}
			return 0;
		}
	}
	/*不能与前面的可用内存归结在一起*/
	if (i<man->frees){
		if (addr+size == man->free[i].addr){
			/*可以与后面的合并在一起*/
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0;
		}
	}
	/*既不能跟前面的合并在一起也不能跟后面的合并在一起*/
	if (man->frees<MEMMAN_FREES){
		for (j=man->frees; j>i; j--){
			man->free[j] = man->free[j-1];
		}
		man->frees ++;
		if (man->maxfrees < man->frees){
			man->maxfrees = man->frees;
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	man->losts ++;
	man->lostsize += size;
	return -1;
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & (0xfffff000);
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & (0xfffff000);
	i = memman_free(man, addr, size);
	return i;
}

