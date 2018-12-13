#include "ldso.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

void display_ldd(struct ELF *my_elf)
{
    char *ld_library_path = ".";

    char *my_elf_str = (void *) my_elf->ehdr;
    ElfW(Dyn) *my_dyn_section = my_elf->dyn;

    int nb_elt_dyn = 1;
    for (ElfW(Dyn) *i = my_dyn_section; i->d_tag != DT_NULL; i++)
        nb_elt_dyn++;

    for (int i = 0; i < nb_elt_dyn; i++)
    {
        if (my_dyn_section->d_tag == DT_NEEDED)
        {
            char *name = my_elf_str + my_elf->shdr_dynstr->sh_offset
                + my_dyn_section->d_un.d_val;
            printf("%*.s%s => %s\n", 8, " ", name, ld_library_path);
        }
        my_dyn_section++;
    }
}
