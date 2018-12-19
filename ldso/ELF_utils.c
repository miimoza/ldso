#include "ldso.h"
#include "string.h"
#include "stdlib.h"


static void free_path_list(struct path_list *my_path_list)
{
    while (my_path_list)
    {
        free(my_path_list->pathname);
        struct path_list *tmp = my_path_list->next;
        free(my_path_list);
        my_path_list = tmp;
    }
}

static void free_elf(struct ELF *my_elf)
{
    free(my_elf);
}

static void free_link_map(struct link_map *my_link_map)
{
    while (my_link_map)
    {
        struct link_map *tmp = my_link_map->l_next;
        free(my_link_map);
        my_link_map = tmp;
    }
}

void free_context(struct Context *my_context)
{
    free_path_list(my_context->library_path_list);
    free_elf(my_context->bin);
    free_link_map(my_context->link_map);
    free(my_context);
}

struct path_list *create_path_node(char *pathname, int len)
{
    struct path_list *my_path_list = malloc(sizeof(struct path_list));
    my_path_list->pathname = malloc(sizeof(char) * (len + 1));
    memcpy(my_path_list->pathname, pathname, len);
    my_path_list->pathname[len] = '\0';

    return my_path_list;
}

static struct path_list *rec_path_list(char *lib_path_var)
{
    int i = 0;
    while (lib_path_var[i] != '\0' && lib_path_var[i] != ':')
        i++;

    struct path_list *my_path_list = create_path_node(lib_path_var, i);

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

/* Return the name of the section from the address of the section header. */
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section)
{
    char *my_elf_str = (char *) my_elf->ehdr;
    ElfW(Shdr) *sh_strtab = &my_elf->shdr[my_elf->ehdr->e_shstrndx];
    return (char *) my_elf_str + sh_strtab->sh_offset + section->sh_name;
}

/* Return the first sub section called <name> in .dynamic section,
if nothing is found, return the last section */
ElfW(Dyn) *get_dyn_section(struct ELF *my_elf , int tag)
{
    ElfW(Dyn) *my_dyn = my_elf->dyn;

    while(!(my_dyn->d_tag & DT_NULL) && !(my_dyn->d_tag & tag))
        my_dyn++;

    return my_dyn;
}

/* Return the first section called <name>, if nothing is found,
return the last section */
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
