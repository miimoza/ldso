#include <elf.h>
#include <link.h>
#include <asm/stat.h>
#include <asm/mman.h>
#include <asm-generic/fcntl.h>

#include "ldso.h"
#include "types.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

struct ELF *elf_loader(char *pathname, void *addr)
{
	struct ELF *my_elf = malloc(sizeof(struct ELF));


	if (pathname) // LOADED BY PATHNAME
	{
		struct stat stat_buffer;
		stat(pathname, &stat_buffer);
		int fd = open(pathname, O_RDONLY);

		my_elf->pathname = malloc(sizeof(char) * strlen(pathname));
		memcpy(my_elf->pathname, pathname, strlen(pathname));

		my_elf->ehdr = mmap(0, stat_buffer.st_size,
	        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
		printf("test%p\n", my_elf->ehdr);

		close(fd);
	}
	else // ALREADY LOADED IN THE MEMORY
	{
		my_elf->ehdr = addr;
	}


	char *my_elf_str = (void *) my_elf->ehdr;
	my_elf->shdr = (void *) my_elf_str + my_elf->ehdr->e_shoff;

	my_elf->phdr = (void *) my_elf_str + my_elf->ehdr->e_phoff;
/*MAP_ANONYMOUS |*/ /*MAP_ANONYMOUS |*/
    ElfW(Shdr) *section = my_elf->shdr;/*MAP_ANONYMOUS |*/
	unsigned i = 0;
    while(i < my_elf->ehdr->e_shnum)
    {
        if (section->sh_type == SHT_DYNAMIC)
        {
            my_elf->dyn = (void *) my_elf_str + section->sh_offset;
            my_elf->shdr_dyn = section;
        }
		if (section->sh_type == SHT_DYNSYM)
        {
            my_elf->dynsym = (void *) my_elf_str + section->sh_offset;
            my_elf->shdr_dynsym = section;
        }

        char *section_str = (void *) section;
        section = (void *) section_str + my_elf->ehdr->e_shentsize;
        i++;
    }

	my_elf->dynstr = (void *) my_elf_str +
		get_section(my_elf, ".dynstr")->sh_offset;
	my_elf->shdr_dynstr = get_section(my_elf, ".dynstr");
	return my_elf;
}

static void set_perm(void *address, size_t len, int flags)
{
	int prot = PROT_NONE;
	if (flags & PF_R)
		prot += PROT_READ;
	if (flags & PF_W)
		prot += PROT_WRITE;
	if (flags & PF_X)
		prot += PROT_EXEC;

	printf("mmap fix:%d\n", mmap(address, len,
		prot, MAP_FIXED, -1, 0));
}

ElfW(Addr) dso_loader(char *pathname, struct link_map *my_link_map)
{
	struct ELF *my_elf = elf_loader(pathname, NULL);

	printf("name:%s\n", my_elf->pathname);
	ElfW(Phdr) *my_phdr = my_elf->phdr;

    for (int i = 0; i < my_elf->ehdr->e_phnum; i++)
    {
        if (my_phdr->p_type == PT_LOAD)
		{
			set_perm((void *) my_phdr->p_vaddr, my_phdr->p_memsz, my_phdr->p_flags);
			printf("LOAD %lx %lx %d\n", my_phdr->p_vaddr, my_phdr->p_memsz, my_phdr->p_flags);
		}

		char *data_str = (void *) my_phdr;
		my_phdr = (void *) &data_str[my_elf->ehdr->e_phentsize];
	}

	my_link_map->l_ld = my_elf->dyn;


	return (ElfW(Addr)) my_elf->ehdr;
}
