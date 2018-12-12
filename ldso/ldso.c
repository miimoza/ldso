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


ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type)
{
	for (; auxv->a_type != AT_NULL; auxv++)
		if (auxv->a_type == type)
			return auxv;
	return NULL;
}

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

void ldso_main(u64 *stack)
{
	int argc = *stack;
	char **argv = (void *)&stack[1];
	char **envp = argv + argc + 1;

	ElfW(auxv_t) *auxv = find_auxv(envp);
	u64 entry = get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val;

	struct stat stat_buffer;
	stat("test-standalone", &stat_buffer);
	ElfW(Ehdr) *my_elf = mmap((void *) entry, stat_buffer.st_size,
        PROT_READ|PROT_WRITE, MAP_PRIVATE, -1, 0);
	printf("sh_off:%lx\n", my_elf->e_phoff);

	my_elf = my_elf;





	jmp_to_usercode(entry, (u64)stack);
}
