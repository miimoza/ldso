struct ELF
{
    ElfW(Ehdr) *ehdr;
    ElfW(Shdr) *shdr;
    ElfW(Phdr) *phdr;
    ElfW(Dyn) *dyn;
    ElfW(Shdr) *shdr_dyn;
    ElfW(Sym) *sym;
};

void display_elf_header(struct ELF *my_elf);
void display_section_header(struct ELF *my_elf);
void display_program_header(struct ELF *my_elf);
void display_symtabs(struct ELF *my_elf);
void display_dynamic_section(struct ELF *my_elf);
