#include "ldso.h"

// Return the name of the section from the address of the section header.
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section)
{
    char *my_elf_str = (char *) my_elf->ehdr;
    ElfW(Shdr) *sh_strtab = &my_elf->shdr[my_elf->ehdr->e_shstrndx];
    return (char *) my_elf_str + sh_strtab->sh_offset + section->sh_name;
}

// Return the first section called <name>, if nothing is found, return the last section
ElfW(Shdr) *get_section(struct ELF *my_elf, char *name)
{
    ElfW(Shdr) *section = my_elf->shdr;
	unsigned i = 0;

    while(i < my_elf->ehdr->e_shnum &&
        !my_str_cmp(get_section_name(my_elf, section), name))
    {
        char *section_str = (void *) section;
        section = (void *) section_str + my_elf->ehdr->e_shentsize;
        i++;
    }

    return section;
}
