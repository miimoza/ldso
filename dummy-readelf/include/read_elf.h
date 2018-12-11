#define ELFW(type)	_ELFW (ELF, __ELF_NATIVE_CLASS, type)
#define _ELFW(e,w,t)	_ELFW_1 (e, w, _##t)
#define _ELFW_1(e,w,t)	e##w##t

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
};

void display_elf_header(struct ELF *my_elf);
void display_section_header(struct ELF *my_elf);
void display_program_header(struct ELF *my_elf);
void display_symtabs(struct ELF *my_elf);
void display_dynamic_section(struct ELF *my_elf);
