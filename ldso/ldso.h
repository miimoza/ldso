#include <elf.h>
#include <link.h>
#include "types.h"

struct ELF
{
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

ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type);

// DISPLAY
void display_auxv(ElfW(auxv_t) *auxv);
void display_ldd(struct ELF *my_elf);


// HELPER
int my_var_cmp(char *a, char *b);
int my_str_cmp(char *a, char *b);
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section);
ElfW(Shdr) *get_section(struct ELF *my_elf, char *name);
