#include <elf.h>
#include <link.h>

#include "unistd.h"
#include "types.h"
#include "ldso.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


static void c_display_addr(char *buf, char *field, u64 value)
{
    sprintf(buf + strlen(buf), "%s:%*.s0x%lx\n", field,
        16 - strlen(field), " ", value);
}

static void c_display_str(char *buf, char *field, u64 value)
{
    sprintf(buf + strlen(buf), "%s:%*.s%s\n", field,
        16 - strlen(field), " ", value);
}

static void c_display(char *buf, char *field, u64 value)
{
    sprintf(buf + strlen(buf), "%s:%*.s%lx\n", field,
        16 - strlen(field), " ", value);
}

void display_auxv(ElfW(auxv_t) *auxv)
{
    char *buf = malloc(sizeof(char) * 512);
    c_display_addr(buf, "AT_SYSINFO_EHDR", get_auxv_entry(auxv, AT_SYSINFO_EHDR)->a_un.a_val);
    c_display(buf, "AT_HWCAP", get_auxv_entry(auxv, AT_HWCAP)->a_un.a_val);
    c_display(buf, "AT_PAGESZ", get_auxv_entry(auxv, AT_PAGESZ)->a_un.a_val);
    c_display(buf, "AT_CLKTCK", get_auxv_entry(auxv, AT_CLKTCK)->a_un.a_val);
    c_display_addr(buf, "AT_PHDR", get_auxv_entry(auxv, AT_PHDR)->a_un.a_val);
    c_display(buf, "AT_PHENT", get_auxv_entry(auxv, AT_PHENT)->a_un.a_val);
    c_display(buf, "AT_PHNUM", get_auxv_entry(auxv, AT_PHNUM)->a_un.a_val);
    c_display_addr(buf, "AT_BASE", get_auxv_entry(auxv, AT_BASE)->a_un.a_val);
    c_display_addr(buf, "AT_FLAGS", get_auxv_entry(auxv, AT_FLAGS)->a_un.a_val);
    c_display_addr(buf, "AT_ENTRY", get_auxv_entry(auxv, AT_ENTRY)->a_un.a_val);
    c_display(buf, "AT_UID", get_auxv_entry(auxv, AT_UID)->a_un.a_val);
    c_display(buf, "AT_EUID", get_auxv_entry(auxv, AT_EUID)->a_un.a_val);
    c_display(buf, "AT_GID", get_auxv_entry(auxv, AT_GID)->a_un.a_val);
    c_display(buf, "AT_EGID", get_auxv_entry(auxv, AT_EGID)->a_un.a_val);
    c_display(buf, "AT_SECURE", get_auxv_entry(auxv, AT_SECURE)->a_un.a_val);
    c_display_addr(buf, "AT_RANDOM", get_auxv_entry(auxv, AT_RANDOM)->a_un.a_val);
    c_display_addr(buf, "AT_HWCAP2", get_auxv_entry(auxv, AT_HWCAP2)->a_un.a_val);
    c_display_str(buf, "AT_EXECFN", get_auxv_entry(auxv, AT_EXECFN)->a_un.a_val);
    c_display_str(buf, "AT_PLATFORM", get_auxv_entry(auxv, AT_PLATFORM)->a_un.a_val);
    write(2, buf, strlen(buf));
}
