#include <elf.h>
#include <link.h>
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
    struct path_list *library_path_list;
    struct ELF *bin;
    struct link_map *link_map;
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
struct ELF *elf_loader(char *pathname);
struct link_map *build_link_map(struct ELF *my_elf);

// DISPLAY
void display_auxv(ElfW(auxv_t) *auxv);
void display_ldd(struct ELF *my_elf);

// HELPER
int my_var_cmp(char *a, char *b);
int my_str_cmp(char *a, char *b);

// UTILS
struct path_list *build_library_path_list(char *lib_path_var);
int get_dyn_num(ElfW(Dyn) *my_dyn);
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section);
ElfW(Shdr) *get_section(struct ELF *my_elf, char *name);
ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type);
