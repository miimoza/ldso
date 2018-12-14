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
#include "string.h"

int env_var_display;

ElfW(auxv_t) *find_auxv(char **envp)
{
	env_var_display = 0;
	while (*envp != NULL)
	{
		if (my_var_cmp(*envp, "LD_SHOW_AUXV=1"))
			env_var_display += VAR_LD_SHOW_AUXV;
		if (my_var_cmp(*envp, "LD_TRACE_LOADED_OBJECTS=1"))
			env_var_display += VAR_LD_TRACE_LOADED_OBJECTS;
		envp++;
	}
	envp++;

	return (ElfW(auxv_t) *)envp;
}

ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type)
{
	for (; auxv->a_type != AT_NULL; auxv++)
		if (auxv->a_type == type)
			return auxv;
	return NULL;
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


	struct ELF *my_elf = elf_loader(argv[0]);

	if (env_var_display & VAR_LD_SHOW_AUXV)
		display_auxv(auxv);
	if (env_var_display & VAR_LD_TRACE_LOADED_OBJECTS)
		display_ldd(my_elf);

	struct link_map *my_link_map = build_link_map(my_elf);
	my_link_map = my_link_map;

	u64 entry = get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val;
	entry = entry;

	printf("end\n");
	jmp_to_usercode(entry, (u64)stack);
}
