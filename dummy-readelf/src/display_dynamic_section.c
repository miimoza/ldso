#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <link.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>

#include "read_elf.h"


void display_dynamic_section(struct ELF *my_elf)
{
    int nb_elt_dyn = my_elf->shdr_dyn->sh_size / my_elf->shdr_dyn->sh_entsize;
    printf("Dynamic section at offset 0x%lx contains %d entries:\n",
        my_elf->shdr_dyn->sh_offset, nb_elt_dyn);
    printf("  Tag        Type                         Name/Value\n");

    ElfW(Dyn) *my_dyn = my_elf->dyn;

    for (int i = 0; i < nb_elt_dyn; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), " 0x%016lx ", my_dyn->d_tag);

        char *type;
        switch(my_dyn->d_tag)
        {
            case DT_NULL:
                type = "NULL";
                break;
            case DT_NEEDED:
                type = "NEEDED";
                break;
            case DT_PLTRELSZ:
                type = "PLTRELSZ";
                break;
            case DT_PLTGOT:
                type = "PLTGOT";
                break;
            case DT_HASH:
                type = "HASH";
                break;
            case DT_STRTAB:
                type = "STRTAB";
                break;
            case DT_SYMTAB:
                type = "SYMTAB";
                break;
            case DT_RELA:
                type = "RELA";
                break;
            case DT_RELASZ:
                type = "RELASZ";
                break;
            case DT_RELAENT:
                type = "RELAENT";
                break;
            case DT_SYMENT:
                type = "SYMENT";
                break;
            case DT_INIT:
                type = "INIT";
                break;
            case DT_FINI:
                type = "FINI";
                break;
            case DT_SONAME:
                type = "SONAME";
                break;
            case DT_RPATH:
                type = "RPATH";
                break;
            case DT_SYMBOLIC:
                type = "SYMBOLIC";
                break;
            case DT_REL:
                type = "REL";
                break;
            case DT_RELSZ:
                type = "RELSZ";
                break;
            case DT_RELENT:
                type = "NEEDED";
                break;
            case DT_PLTREL:
                type = "PLTREL";
                break;
            case DT_DEBUG:
                type = "DEBUG";
                break;
            case DT_JMPREL:
                type = "JMPREL";
                break;
            case DT_BIND_NOW:
                type = "BIND_NOW";
                break;
            case DT_RUNPATH:
                type = "RUNPATH";
                break;
            case DT_LOPROC:
            case DT_HIPROC:
                type = "OC";
                break;
            default:
                type = "";
                break;
        }

        sprintf(line + strlen(line), "(%s)%*.s", type, 18 - (int) strlen(type), " ");

        sprintf(line + strlen(line), " 0x%lx", my_dyn->d_un.d_val);

        printf("%s\n", line);

        free(line);

        my_dyn++;
    }
}
