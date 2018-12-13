#include <stdbool.h>
#include <elf.h>
#include <link.h>
#include <stddef.h>
#include <sys/auxv.h>
#include <asm/mman.h>
#include <asm-generic/fcntl.h>
#include <asm/stat.h>

#include "ldso.h"
#include "types.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

ElfW(auxv_t) *find_auxv(char **envp)
{
	unsigned auxv_display_enable = 0;
	while (*envp != NULL)
	{
		if (my_var_cmp(*envp, "LD_SHOW_AUXV=1"))
			auxv_display_enable = 1;
		envp++;
	}
	envp++;

	if (auxv_display_enable)
		display_auxv((ElfW(auxv_t) *)envp);

	return (ElfW(auxv_t) *)envp;
}

static inline void jmp_to_usercode(u64 entry, u64 stack)
{
	asm volatile ("mov %[stack], %%rsp\n"
		      "push %[entry]\n"
		      "ret" :: [entry]"r"(entry), [stack]"r"(stack));
}

static struct ELF *elf_loader(char *pathname)
{
	struct stat stat_buffer;
	stat(pathname, &stat_buffer);
	int fd = open(pathname, O_RDONLY);
	struct ELF *my_elf = malloc(sizeof(struct ELF));
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

void ldso_main(u64 *stack)
{
	int argc = *stack;
	char **argv = (void *)&stack[1];
	char **envp = argv + argc + 1;

	ElfW(auxv_t) *auxv = find_auxv(envp);
	u64 entry = get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val;

	struct ELF *my_elf = elf_loader(argv[0]);
	display_ldd(my_elf);

	jmp_to_usercode(entry, (u64)stack);
}
