#include <link.h>

#include "ldso.h"
#include "stdlib.h"
#include "stdio.h"


static void print_link_map(struct link_map *my_link_map)
{
    while (my_link_map)
    {
        printf("l_addr:%lx\n", my_link_map->l_addr);
        printf("l_name:%s\n", my_link_map->l_name);
        printf("l_ld:%lx\n", my_link_map->l_ld);
        printf("l_prev:%lx\n", my_link_map->l_prev);
        printf("l_next:%lx\n", my_link_map->l_next);
        printf("\n");
        my_link_map = my_link_map->l_next;
    }
}

static struct link_map *last_link_map(struct link_map *my_link_map)
{
    while (my_link_map->l_next)
        my_link_map = my_link_map->l_next;
    return my_link_map;
}

static struct link_map *rec_load_library(char *lib_filename, struct link_map *prev)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));

    struct ELF *my_lib = elf_loader(lib_filename);

    my_link_map->l_addr = (ElfW(Addr)) my_lib->ehdr;
    my_link_map->l_name = lib_filename;
    my_link_map->l_ld = my_lib->dyn;
    my_link_map->l_prev = prev;
    my_link_map->l_next = NULL;

    char *my_lib_str = (void *) my_lib->ehdr;
    ElfW(Dyn) *my_dyn_section = my_lib->dyn;

    int nb_elt_dyn = get_dyn_num(my_dyn_section);

    struct link_map *current_link_map = my_link_map;
    for (int i = 0; i < nb_elt_dyn; i++)
    {
        if (my_dyn_section->d_tag == DT_NEEDED)
        {
            char *lib_filename = my_lib_str + my_lib->shdr_dynstr->sh_offset
                + my_dyn_section->d_un.d_val;
            current_link_map->l_next = rec_load_library(lib_filename, my_link_map);
            current_link_map = last_link_map(current_link_map);
        }
        my_dyn_section++;
    }

    return my_link_map;
}

struct link_map *load_binary(struct ELF *my_elf)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    my_link_map->l_addr = (ElfW(Addr)) my_elf->ehdr;
    my_link_map->l_name = my_elf->pathname;
    my_link_map->l_ld = my_elf->dyn;
    my_link_map->l_prev = NULL;
    my_link_map->l_next = NULL;

    return my_link_map;
}

struct link_map *load_ldso(struct ELF *my_elf, struct link_map *prev)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    char *my_elf_str = (void *) my_elf->ehdr;
    char *ldso_filename = (char *)
        &my_elf_str[get_section(my_elf, ".interp")->sh_offset];
    struct ELF *my_ldso = elf_loader(ldso_filename);
    my_link_map->l_addr = (ElfW(Addr)) my_ldso->ehdr;
    my_link_map->l_name = ldso_filename;
    my_link_map->l_ld = my_ldso->dyn;
    my_link_map->l_prev = prev;
    my_link_map->l_next = NULL;

    return my_link_map;
}


struct link_map *build_link_map(struct ELF *my_elf)
{
    struct link_map *my_link_map = load_binary(my_elf);
    struct link_map *ret_link_map = my_link_map;
    my_link_map->l_next = load_ldso(my_elf, my_link_map);
    my_link_map = my_link_map->l_next;

    ElfW(Dyn) *my_dyn_section = my_elf->dyn;

    int nb_elt_dyn = get_dyn_num(my_dyn_section);
    char *my_elf_str = (void *) my_elf->ehdr;

    for (int i = 0; i < nb_elt_dyn; i++)
    {
        if (my_dyn_section->d_tag == DT_NEEDED)
        {
            char *lib_filename = my_elf_str + my_elf->shdr_dynstr->sh_offset
                + my_dyn_section->d_un.d_val;
            my_link_map->l_next = rec_load_library(lib_filename, my_link_map);
            my_link_map = last_link_map(my_link_map);
        }
        my_dyn_section++;
    }

    print_link_map(ret_link_map);
    return ret_link_map;
}
