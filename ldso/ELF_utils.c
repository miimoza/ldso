#include "ldso.h"
#include "string.h"
#include "stdlib.h"

struct path_list *rec_path_list(char *lib_path_var)
{
    int i = 0;
    while (lib_path_var[i] != '\0' && lib_path_var[i] != ':')
        i++;

    struct path_list *my_path_list = malloc(sizeof(struct path_list));
    my_path_list->pathname = malloc(sizeof(char) * (i + 1));
    memcpy(my_path_list->pathname, lib_path_var, i);
    my_path_list->pathname[i] = '\0';

    if (lib_path_var[i] == ':')
        my_path_list->next = rec_path_list(lib_path_var + i + 1);

    return my_path_list;
}


// Fill the path_list structure with the string, each path being separate by :
struct path_list *build_library_path_list(char *lib_path_var)
{
    int i = 0;
    while (lib_path_var[i] != '=')
        i++;

    return rec_path_list(lib_path_var + i + 1);
}

// Return number of elements in the dynamic table
int get_dyn_num(ElfW(Dyn) *my_dyn_section)
{
    int nb_elt_dyn = 1;
    for (ElfW(Dyn) *i = my_dyn_section; i->d_tag != DT_NULL; i++)
        nb_elt_dyn++;
    return nb_elt_dyn;
}

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
