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

		close(fd);
	}
	else // ALREADY LOADED IN THE MEMORY
	{
		my_elf->ehdr = addr;
	}


	char *my_elf_str = (void *) my_elf->ehdr;
	my_elf->shdr = (void *) my_elf_str + my_elf->ehdr->e_shoff;

	my_elf->phdr = (void *) my_elf_str + my_elf->ehdr->e_phoff;

    ElfW(Shdr) *section = my_elf->shdr;
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

static void set_perm(void *my_dso, void *address, size_t len, int flags, int fd, size_t offset)
{
	int prot = PROT_NONE;
	if (flags & PF_R)
		prot += PROT_READ;
	if (flags & PF_W)
		prot += PROT_WRITE;
	if (flags & PF_X)
		prot += PROT_EXEC;

	printf("mmap fix:%d\n", mmap((char *) my_dso + (size_t) address, len,
		prot, MAP_FIXED, fd, offset));
}

void *dso_loader(struct ELF *my_elf, char *pathname)
{
	printf("dso_loader: %s\n", pathname);

	int fd = open(pathname, O_RDONLY);
	struct stat stat_buffer;
	stat(pathname, &stat_buffer);
	void *my_dso = mmap(0, stat_buffer.st_size, PROT_READ, MAP_ANONYMOUS, -1, 0);


	ElfW(Phdr) *my_phdr = my_elf->phdr;
    for (int i = 0; i < my_elf->ehdr->e_phnum; i++)
    {
        if (my_phdr->p_type == PT_LOAD)
		{
			printf("LOAD SEGMENT.. [vaddr:%016lx memsz:%016lx flags:%d]\n",
				my_phdr->p_vaddr, my_phdr->p_memsz, my_phdr->p_flags);
			set_perm(my_dso, (void *) my_phdr->p_vaddr, my_phdr->p_filesz,
				my_phdr->p_flags, fd, my_phdr->p_offset);
		}

		char *data_str = (void *) my_phdr;
		my_phdr = (void *) &data_str[my_elf->ehdr->e_phentsize];
	}

	close(fd);

	return my_dso;
}
