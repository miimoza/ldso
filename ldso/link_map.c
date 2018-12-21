#include <link.h>
#include <sys/stat.h>

#include "ldso.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"


static void display_ldd(struct link_map *my_link_map)
{
    while (my_link_map)
    {
        printf("	%s => %s (0x%016lx)\n",
            get_name_from_path(my_link_map->l_name),
            my_link_map->l_name, my_link_map->l_addr);
        my_link_map = my_link_map->l_next;
    }
}

static struct link_map *last_link_map(struct link_map *my_link_map)
{
    while (my_link_map->l_next)
        my_link_map = my_link_map->l_next;
    return my_link_map;
}

static char *find_in_dir(char *filename, char *directory)
{
    char *path = malloc(sizeof(char) * strlen(filename)
        + strlen(directory) + 1);
    memcpy(path, directory, strlen(directory));
    memcpy(path + strlen(directory), "/", 1);
    memcpy(path + strlen(directory) + 1, filename, strlen(filename));

    struct stat stat_buffer;
    int res = stat(path, &stat_buffer);
    if (res >= 0 && stat_buffer.st_mode & S_IXUSR)
        return path;
    return NULL;
}

static struct link_map *rec_load_library(char *lib_filename,
    struct path_list *library_path, struct link_map *prev)
{
    if (!library_path)
        library_path = create_path_node(".", 1);

    char *path = NULL;
    struct path_list *tmp = library_path;
    while (library_path &&
        !(path = find_in_dir(lib_filename, library_path->pathname)))
        library_path = library_path->next;
    library_path = tmp;

    if (!path)
    {
        printf("Error: Library %s is not found.\n", lib_filename);
        _exit(1);
    }

    struct link_map *my_link_map = malloc(sizeof(struct link_map));

    struct ELF *my_lib = elf_loader(path, NULL);
    my_link_map->l_addr = (ElfW(Addr)) my_lib->ehdr;
    my_link_map->l_name = path;
    my_link_map->l_ld = dso_loader(my_lib, path);
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
            current_link_map->l_next = rec_load_library(lib_filename,
                library_path, my_link_map);
            current_link_map = last_link_map(current_link_map);
        }
        my_dyn_section++;
    }

    return my_link_map;
}

static void load_libraries(struct ELF *my_elf,
    struct path_list *library_path, struct link_map *my_link_map)
{
    ElfW(Dyn) *my_dyn_section = my_elf->dyn;

    int nb_elt_dyn = get_dyn_num(my_dyn_section);
    char *my_elf_str = (void *) my_elf->ehdr;
    for (int i = 0; i < nb_elt_dyn; i++)
    {
        if (my_dyn_section->d_tag == DT_NEEDED)
        {
            char *lib_filename = my_elf_str + my_elf->shdr_dynstr->sh_offset
                + my_dyn_section->d_un.d_val;
            my_link_map->l_next = rec_load_library(lib_filename,
                library_path, my_link_map);
            my_link_map = last_link_map(my_link_map);
        }
        my_dyn_section++;
    }
}

static struct link_map *load_binary(struct ELF *my_elf)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    my_link_map->l_addr = (ElfW(Addr)) my_elf->ehdr;
    my_link_map->l_name = my_elf->pathname;
    my_link_map->l_ld = dso_loader(my_elf, my_elf->pathname);
    my_link_map->l_prev = NULL;
    my_link_map->l_next = NULL;

    return my_link_map;
}

static struct link_map *load_ldso(struct ELF *my_elf, struct link_map *prev)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    char *my_elf_str = (void *) my_elf->ehdr;

    char *ldso_filename = (char *) my_elf_str
        + get_section(my_elf, ".interp")->sh_offset;

    struct ELF *my_ldso = elf_loader(ldso_filename, NULL);
    my_link_map->l_addr = (ElfW(Addr)) my_ldso->ehdr;
    my_link_map->l_ld = dso_loader(my_ldso, ldso_filename);
    my_link_map->l_name = ldso_filename;
    my_link_map->l_prev = prev;
    my_link_map->l_next = NULL;

    return my_link_map;
}

static struct link_map *load_vdso(struct Context *my_context,
    struct link_map *prev)
{
    struct link_map *my_link_map = malloc(sizeof(struct link_map));
    void *vdso_addr =
        (void *) get_auxv_entry(my_context->auxv, AT_SYSINFO_EHDR)->a_un.a_val;

    struct ELF *my_vdso = elf_loader(NULL, vdso_addr);

    char *name;
    switch (my_vdso->ehdr->e_machine)
    {
        case EM_PPC:
        case EM_S390:
            name = "linux-vdso32.so.1";
            break;
        case EM_PPC64:
            name = "linux-vdso64.so.1";
            break;
        case EM_SH:
        case EM_IA_64:
            name = "linux-gate.so.1";
            break;
        case EM_X86_64:
            name = "linux-vdso.so.1";
            break;
        default:
            name = "Unknown Arch: vdso path not found.";
    }

    my_link_map->l_addr = (ElfW(Addr)) my_vdso->ehdr;
    my_link_map->l_name = name;
    my_link_map->l_ld = dso_loader(my_vdso, name);
    my_link_map->l_prev = prev;
    my_link_map->l_next = NULL;

    return my_link_map;
}


struct link_map *build_link_map(struct Context *my_context, struct ELF *my_elf,
    struct path_list *library_path)
{
    struct link_map *my_link_map = load_binary(my_elf);
    struct link_map *ret_link_map = my_link_map;

    my_link_map->l_next = load_ldso(my_elf, my_link_map);
    my_link_map = my_link_map->l_next;

    my_link_map->l_next = load_vdso(my_context, my_link_map);
    my_link_map = my_link_map->l_next;

    my_context->library_link_map = my_link_map;

    load_libraries(my_elf, library_path, my_link_map);

    if (my_context->env_var_display & VAR_LD_TRACE_LOADED_OBJECTS)
        display_ldd(ret_link_map);
    return ret_link_map;
}
