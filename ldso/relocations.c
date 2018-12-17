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

void apply_relocations(struct Context *my_context)
{
    printf("got relocation address (jmprel):%lx\n", get_dyn_entry(my_context->bin->dyn, DT_JMPREL)->d_un.d_val);

    //printf("rel:%lx\n", get_dyn_entry(my_elf->dyn, DT_REL)->d_un.d_val);
    //printf("rela:%lx\n", get_dyn_entry(my_elf->dyn, DT_RELA)->d_un.d_val);
    //printf("hash:%lx\n", get_dyn_entry(my_elf->dyn, DT_HASH)->d_un.d_val);


	struct ELF *my_elf = my_context->bin;


	int nb_elt_dynsym = my_elf->shdr_dynsym->sh_size / my_elf->shdr_dynsym->sh_entsize;

	ElfW(Sym) *my_dynsym = my_elf->dynsym;
	for (int i = 0; i < nb_elt_dynsym; i++, my_dynsym++)
	{
		if (!my_dynsym->st_value)
		{

			char *symbol =  (char *) my_elf->ehdr +
				get_section(my_elf, ".dynstr")->sh_offset + my_dynsym->st_name;

			ElfW(Addr) address = find_sym_address(my_context->library_link_map, symbol);
			printf("address found for %s : %lx\n", symbol, address);
		}
	}



	/*
	ElfW(Dyn) *plt_relocations = (ElfW(Dyn) *) get_dyn_entry(my_elf->dyn, DT_JMPREL)->d_un.d_val;

	for (int i = 0; i < 40; i++)
		printf("ht%d:%lx\n",i, plt_relocations[i]);
		*/


    printf("hey\n");
}
