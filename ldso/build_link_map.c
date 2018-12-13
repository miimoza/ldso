#include <link.h>

#include "ldso.h"
#include "stdlib.h"
#include "stdio.h"


static void print_link_map(struct link_map *my_link_map)
{
    while (my_link_map->l_next)
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

static struct link_map *rec_load_library(char *lib_filename, struct link_map *prev)
{
    printf("About to load %s\n", lib_filename);
    struct link_map *my_link_map = malloc(sizeof(struct link_map));

    /*
    my_link_map->l_addr =
    my_link_map->l_name =
    my_link_map->l_ld =
    */
    my_link_map->l_prev = prev;

    return my_link_map;
}

static struct link_map *load_libraries(struct ELF *my_elf, struct link_map *my_link_map)
{
    //char *ld_library_path = ".";

    char *my_elf_str = (void *) my_elf->ehdr;
    ElfW(Dyn) *my_dyn_section = my_elf->dyn;

    int nb_elt_dyn = 1;
    for (ElfW(Dyn) *i = my_dyn_section; i->d_tag != DT_NULL; i++)
        nb_elt_dyn++;

    for (int i = 0; i < nb_elt_dyn; i++)
    {
        if (my_dyn_section->d_tag == DT_NEEDED)
        {
            char *lib_filename = my_elf_str + my_elf->shdr_dynstr->sh_offset
                + my_dyn_section->d_un.d_val;
            my_link_map = rec_load_library(lib_filename, my_link_map);
        }
        my_dyn_section++;
    }

    return my_link_map;
}


struct link_map *build_link_map(struct ELF *my_elf)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    my_elf->link_map = my_link_map;
    my_link_map->l_addr = (ElfW(Addr)) my_elf->ehdr;
    my_link_map->l_name = my_elf->name;
    my_link_map->l_ld = my_elf->dyn;
    my_link_map->l_prev = NULL;


    // (char *) &my_elf_str[get_section(my_elf, ".interp")->sh_offset]);
    print_link_map(my_link_map);
    my_link_map->l_next = load_libraries(my_elf, my_link_map);

    return my_elf->link_map;
}
