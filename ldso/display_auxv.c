#include <elf.h>
#include <link.h>

#include "types.h"
#include "ldso.h"
#include "stdio.h"
#include "string.h"

static void c_display_addr(char *field, u64 value)
{
    printf("%s:%*.s0x%lx\n", field, 16 - strlen(field), " ", value);
}

static void c_display_str(char *field, u64 value)
{
    printf("%s:%*.s%s\n", field, 16 - strlen(field), " ", value);
}

static void c_display(char *field, u64 value)
{
    printf("%s:%*.s%lx\n", field, 16 - strlen(field), " ", value);
}




void display_auxv(ElfW(auxv_t) *auxv)
{
    c_display_addr("AT_SYSINFO_EHDR", get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val);
    c_display("AT_HWCAP", get_auxv_entry(auxv, AT_HWCAP)->a_un.a_val);
    c_display("AT_PAGESZ", get_auxv_entry(auxv, AT_PAGESZ)->a_un.a_val);
    c_display("AT_CLKTCK", get_auxv_entry(auxv, AT_CLKTCK)->a_un.a_val);
    c_display_addr("AT_PHDR", get_auxv_entry(auxv, AT_PHDR)->a_un.a_val);
    c_display("AT_PHENT", get_auxv_entry(auxv, AT_PHENT)->a_un.a_val);
    c_display("AT_PHNUM", get_auxv_entry(auxv, AT_PHNUM)->a_un.a_val);
    c_display_addr("AT_BASE", get_auxv_entry(auxv, AT_BASE)->a_un.a_val);
    c_display_addr("AT_FLAGS", get_auxv_entry(auxv, AT_FLAGS)->a_un.a_val);
    c_display_addr("AT_ENTRY", get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val);
    c_display("AT_UID", get_auxv_entry(auxv, AT_UID)->a_un.a_val);
    c_display("AT_EUID", get_auxv_entry(auxv, AT_EUID)->a_un.a_val);
    c_display("AT_GID", get_auxv_entry(auxv, AT_GID)->a_un.a_val);
    c_display("AT_EGID", get_auxv_entry(auxv, AT_EGID)->a_un.a_val);
    c_display("AT_SECURE", get_auxv_entry(auxv, AT_SECURE)->a_un.a_val);
    c_display_addr("AT_RANDOM", get_auxv_entry(auxv, AT_RANDOM)->a_un.a_val);
    c_display_addr("AT_HWCAP2", get_auxv_entry(auxv, AT_HWCAP2)->a_un.a_val);
    c_display_str("AT_EXECFN", get_auxv_entry(auxv, AT_EXECFN)->a_un.a_val);
    c_display_str("AT_PLATFORM", get_auxv_entry(auxv, AT_PLATFORM)->a_un.a_val);
}
