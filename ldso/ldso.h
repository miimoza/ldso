#ifndef LDSO_H
#define LDSO_H

#include <elf.h>
#include <link.h>

#define ELFW(type)	_ELFW (ELF, __ELF_NATIVE_CLASS, type)
#define _ELFW(e,w,t)	_ELFW_1 (e, w, _##t)
#define _ELFW_1(e,w,t)	e##w##t

#include "types.h"


enum env_vars
{
    VAR_LD_SHOW_AUXV = 1,
    VAR_LD_TRACE_LOADED_OBJECTS
};

struct path_list
{
    char *pathname;
    struct path_list *next;
};

struct Context
{
    ElfW(auxv_t) *auxv;
    int env_var_display;
    struct path_list *library_path_list;
    struct ELF *bin;
    struct link_map *link_map;
    struct link_map *library_link_map;
};

struct ELF
{
    char *pathname;
    ElfW(Ehdr) *ehdr;
    ElfW(Shdr) *shdr;
    ElfW(Phdr) *phdr;
    ElfW(Dyn) *dyn;
    ElfW(Shdr) *shdr_dyn;
    ElfW(Sym) *dynsym;
    ElfW(Shdr) *shdr_dynsym;
    ElfW(Sym) *symtab;
    ElfW(Shdr) *shdr_symtab;
    ElfW(Sym) *dynstr;
    ElfW(Shdr) *shdr_dynstr;
};

// MAIN FUNCTIONS

struct link_map *build_link_map(struct Context *my_context,
    struct ELF *my_elf, struct path_list *library_path);
void apply_relocations(struct Context *my_context);


// LOADER AND MAPPER
ElfW(Addr) dso_loader(char *pathname, struct link_map *my_link_map);
struct ELF *elf_loader(char *pathname, void *addr);

// DISPLAY
void display_auxv(ElfW(auxv_t) *auxv);

// FREE
void free_context(struct Context *my_context);

// HELPER
char *get_name_from_path(char *path);
int my_var_cmp(char *a, char *b);
int my_str_cmp(char *a, char *b);

// PATH LISTS
struct path_list *build_library_path_list(char *lib_path_var);
struct path_list *create_path_node(char *pathname, int len);

// UTILS
int get_dyn_num(ElfW(Dyn) *my_dyn);
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section);
ElfW(Dyn) *get_dyn_section(struct ELF *my_elf , int tag);
ElfW(Shdr) *get_section(struct ELF *my_elf, char *name);
ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type);

#endif /* LDSO_H */
