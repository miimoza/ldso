#include "ldso.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static ElfW(Dyn) *get_dyn_entry(ElfW(Dyn) *my_dyn, u32 type)
{
	for (; my_dyn->d_tag != DT_NULL; my_dyn++)
		if (my_dyn->d_tag == type)
			return my_dyn;
	return NULL;
}

static ElfW(Addr) get_sym_address(struct ELF *my_elf, char *sym_name, void *my_dso)
{
	int nb_elt_dynsym = my_elf->shdr_dynsym->sh_size / my_elf->shdr_dynsym->sh_entsize;

	ElfW(Sym) *my_dynsym = my_elf->dynsym;
	for (int i = 0; i < nb_elt_dynsym; i++, my_dynsym++)
	{
		char *symbol =  (char *) my_elf->ehdr +
			get_section(my_elf, ".dynstr")->sh_offset + my_dynsym->st_name;
		if (my_dynsym->st_value && my_str_cmp(sym_name, symbol))
			return (ElfW(Addr)) (char *) my_dso + my_dynsym->st_value;

	}
	return 0;
}

static ElfW(Addr) find_sym_address(struct link_map *my_link_map, char *sym_name)
{
	ElfW(Addr) address;
	struct ELF *my_lib = elf_loader(NULL, (void *) my_link_map->l_addr);
	while (my_link_map && !(address = get_sym_address(my_lib, sym_name, my_link_map->l_ld)))
	{
		my_link_map = my_link_map->l_next;
		if (my_link_map)
			my_lib = elf_loader(NULL, (void *) my_link_map->l_addr);
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
	switch(ELFW(R_TYPE)(reloc_table->r_info))
	{
		case R_386_JMP_SLOT:
			return get_symtab_val(my_elf, ELFW(R_SYM)(reloc_table->r_info));
			break;
		default:
			return get_symtab_val(my_elf, ELFW(R_SYM)(reloc_table->r_info));
	}
}

static void resolve_dso(struct link_map *library_link_map, struct ELF *my_elf,
	void *my_dso)
{
	ElfW(Rela) *reloc_table = (void *)(my_dso +
		get_dyn_entry(my_elf->dyn, DT_JMPREL)->d_un.d_val);

	size_t size_rel = (size_t) get_dyn_entry(my_elf->dyn, DT_PLTRELSZ)->d_un.d_val;
	size_t nb_elt = size_rel / sizeof(ElfW(Rela));

	for (size_t i = 0; i < nb_elt; i++)
	{
		char *symbol = (char *) get_sym(my_elf, reloc_table);
		size_t *target_address = (void *) my_dso + reloc_table->r_offset;
		size_t address = find_sym_address(library_link_map, symbol);
		*target_address = address;
		reloc_table++;
	}
}

void apply_relocations(struct Context *my_context)
{
	struct link_map *my_link_map = my_context->link_map;

	struct ELF *my_elf = elf_loader(NULL, (void *) my_link_map->l_addr);

	while (my_link_map)
	{
		if (get_dyn_entry(my_elf->dyn, DT_JMPREL))
		{
			void *my_dso;
			if (!my_link_map->l_prev)
	            my_dso = (char *)(0);
			else
				my_dso = my_link_map->l_ld;

			resolve_dso(my_context->library_link_map, my_elf, my_dso);
		}
		my_link_map = my_link_map->l_next;
		if (my_link_map)
			my_elf = elf_loader(NULL, (void *) my_link_map->l_addr);
	}
}
