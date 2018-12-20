#include "ldso.h"
#include "stdio.h"
#include "stdlib.h"

static ElfW(Dyn) *get_dyn_entry(ElfW(Dyn) *my_dyn, u32 type)
{
	for (; my_dyn->d_tag != DT_NULL; my_dyn++)
		if (my_dyn->d_tag == type)
			return my_dyn;
	return NULL;
}

static ElfW(Addr) get_sym_address(struct ELF *my_elf, char *sym_name)
{
	int nb_elt_dynsym = my_elf->shdr_dynsym->sh_size / my_elf->shdr_dynsym->sh_entsize;

	ElfW(Sym) *my_dynsym = my_elf->dynsym;
	for (int i = 0; i < nb_elt_dynsym; i++, my_dynsym++)
	{
		char *symbol =  (char *) my_elf->ehdr +
			get_section(my_elf, ".dynstr")->sh_offset + my_dynsym->st_name;
		if (my_dynsym->st_value && my_str_cmp(sym_name, symbol))
			return my_dynsym->st_value;
	}

	return 0;
}

static ElfW(Addr) find_sym_address(struct link_map *my_link_map, char *sym_name)
{
	ElfW(Addr) address;
	struct ELF *my_lib = elf_loader(NULL, (void *) my_link_map->l_addr);
	while (my_link_map && !(address = get_sym_address(my_lib, sym_name)))
	{
		my_lib = elf_loader(NULL, (void *) my_link_map->l_addr);
		my_link_map = my_link_map->l_next;
	}

	return address;
}


static char *get_symtab_val(struct ELF *my_elf, int index)
{
	ElfW(Sym) *my_dynsym = my_elf->dynsym;
	for (int i = 0; i < index; i++)
		my_dynsym++;

	char *symbol = (char *) my_elf->ehdr
		+ get_section(my_elf, ".dynstr")->sh_offset + my_dynsym->st_name;

	return symbol;
}

static char *get_sym(struct ELF *my_elf, ElfW(Rela) *reloc_table)
{
	switch(ELF64_R_TYPE(reloc_table->r_info))
	{
		case R_386_JMP_SLOT:
			return get_symtab_val(my_elf, ELF64_R_SYM(reloc_table->r_info));
			break;
		default:
			return get_symtab_val(my_elf, ELF64_R_SYM(reloc_table->r_info));
	}
}

void apply_relocations(struct Context *my_context)
{
	struct ELF *my_elf = my_context->bin;

	printf("got relocation address (jmprel):%lx\n", get_dyn_entry(my_context->bin->dyn, DT_JMPREL)->d_un.d_val);

	ElfW(Rela) *reloc_table = (ElfW(Rela) *) get_dyn_entry(my_elf->dyn, DT_JMPREL)->d_un.d_val;

	size_t size_rel = (size_t) get_dyn_entry(my_elf->dyn, DT_PLTRELSZ)->d_un.d_val;
	size_t nb_elt = size_rel / sizeof(ElfW(Rela));
	printf("nb:%d\n", size_rel);
	printf("nb:%d\n", nb_elt);


	for (size_t i = 0; i < nb_elt; i++)
	{
		char *symbol = (char *) get_sym(my_elf, reloc_table);
		char *target_address = (void *) reloc_table->r_offset;

		printf("[%s]\n         target addr:%lx\n", symbol, target_address);

		printf("siceskeujpense:%lx\n", (void *) *target_address);

		ElfW(Addr) address = find_sym_address(my_context->library_link_map, symbol);
		printf("	 address found for (%s) : %lx\n", symbol, address);

		*target_address = address;
		printf("	 new target addr:%lx\n\n", target_address);

		printf("siceskeujpense:%lx\n", (void *) *target_address);

		reloc_table++;
	}


    printf("hey\n");
}
