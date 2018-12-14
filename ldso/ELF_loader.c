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

struct ELF *elf_loader(char *pathname)
{
	struct stat stat_buffer;
	stat(pathname, &stat_buffer);
	int fd = open(pathname, O_RDONLY);
	struct ELF *my_elf = malloc(sizeof(struct ELF));

	my_elf->name = malloc(sizeof(char) * strlen(pathname));
	memcpy(my_elf->name, pathname, strlen(pathname));

	my_elf->ehdr = mmap(0, stat_buffer.st_size,
        PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);

	char *shdr_str = (void *) my_elf->ehdr;
	my_elf->shdr = (void *) shdr_str + my_elf->ehdr->e_shoff;

    ElfW(Shdr) *section = my_elf->shdr;
	unsigned i = 0;
    while(i < my_elf->ehdr->e_shnum)
    {
        if (section->sh_type == SHT_DYNAMIC)
        {
            my_elf->dyn = (void *) shdr_str + section->sh_offset;
            my_elf->shdr_dyn = section;
        }
		if (section->sh_type == SHT_DYNSYM)
        {
            my_elf->dynsym = (void *) shdr_str + section->sh_offset;
            my_elf->shdr_dynsym = section;
        }

        char *section_str = (void *) section;
        section = (void *) section_str + my_elf->ehdr->e_shentsize;
        i++;
    }

	my_elf->dynstr = (void *) shdr_str + get_section(my_elf, ".dynstr")->sh_offset;
	my_elf->shdr_dynstr = get_section(my_elf, ".dynstr");
	return my_elf;
}
