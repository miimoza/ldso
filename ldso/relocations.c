#include "ldso.h"
#include "stdio.h"
#include "stdlib.h"

static ElfW(Dyn) *get_dyn_entry(ElfW(Dyn) *my_dyn, u32 type)
{
	for (; my_dyn->d_tag != DT_NULL; my_dyn++)
		if (my_dyn->d_tag == type)
			return my_dyn;
	return NULL;
}

void test(struct ELF *my_elf)
{
    my_elf = my_elf;

    printf("jmprel:%lx\n", get_dyn_entry(my_elf->dyn, DT_JMPREL)->d_un.d_val);

    printf("rel:%lx\n", get_dyn_entry(my_elf->dyn, DT_REL)->d_un.d_val);
    printf("rela:%lx\n", get_dyn_entry(my_elf->dyn, DT_RELA)->d_un.d_val);
    printf("hash:%lx\n", get_dyn_entry(my_elf->dyn, DT_HASH)->d_un.d_val);

    ElfW(Dyn) *hash_table = (ElfW(Dyn) *) get_dyn_entry(my_elf->dyn, DT_GNU_HASH)->d_un.d_ptr;

    printf("hey\n");


}
