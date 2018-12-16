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
            case DT_STRSZ:
                type = "STRSZ";
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
            case DT_TEXTREL:
                type = "TEXTREL";
                break;
            case DT_JMPREL:
                type = "JMPREL";
                break;
            case DT_BIND_NOW:
                type = "BIND_NOW";
                break;
            case DT_INIT_ARRAY:
                type = "INIT_ARRAY";
                break;
            case DT_FINI_ARRAY:
                type = "FINI_ARRAY";
                break;
            case DT_INIT_ARRAYSZ:
                type = "INIT_ARRAYSZ";
                break;
            case DT_FINI_ARRAYSZ:
                type = "FINI_ARRAYSZ";
                break;
            case DT_RUNPATH:
                type = "RUNPATH";
                break;
            case DT_FLAGS:
                type = "FLAGS";
                break;
            case DT_ENCODING:
                type = "ENCODING";
                break;
            case DT_PREINIT_ARRAYSZ:
                type = "PREINIT_ARRAYSZ";
                break;
            case DT_SYMTAB_SHNDX:
                type = "SYMTAB_SHNDX";
                break;
            case DT_NUM:
                type = "NUM";
                break;
            case DT_LOOS:
                type = "LOOS";
                break;
            case DT_HIOS:
                type = "HIOS";
                break;
            case DT_LOPROC:
                type = "LOPROC";
                break;
            case DT_HIPROC:
                type = "HIPROC";
                break;
            case DT_PROCNUM:
                type = "PROCNUM";
                break;
            case DT_GNU_PRELINKED:
                type = "GNU_PRELINKED";
                break;
            case DT_GNU_CONFLICTSZ:
                type = "GNU_CONFLICTSZ";
                break;
            case DT_GNU_LIBLISTSZ:
                type = "GNU_PRELINKED";
                break;
            case DT_CHECKSUM:
                type = "CHECKSUM";
                break;
            case DT_PLTPADSZ:
                type = "PLTPADSZ";
                break;
            case DT_MOVEENT:
                type = "MOVEENT";
                break;
            case DT_FEATURE_1:
                type = "FEATURE_1";
                break;
            case DT_POSFLAG_1:
                type = "POSFLAG_1";
                break;
            case DT_SYMINSZ:
                type = "SYMINSZ";
                break;
            case DT_SYMINENT:
                type = "SYMINENT";
                break;
            case DT_ADDRRNGLO:
                type = "ADDRRNGLO";
                break;
            case DT_GNU_HASH:
                type = "GNU_HASH";
                break;
            case DT_TLSDESC_PLT:
                type = "TLSDESC_PLT";
                break;
            case DT_GNU_CONFLICT:
                type = "GNU_CONFLICT";
                break;
            case DT_GNU_LIBLIST:
                type = "GNU_LIBLIST";
                break;
            case DT_CONFIG:
                type = "CONFIG";
                break;
            case DT_DEPAUDIT:
                type = "DEPAUDIT";
                break;
            case DT_AUDIT:
                type = "AUDIT";
                break;
            case DT_PLTPAD:
                type = "PLTPAD";
                break;
            case DT_MOVETAB:
                type = "MOVETAB";
                break;
            case DT_SYMINFO:
                type = "SYMINFO";
                break;
            case DT_VERSYM:
                type = "VERSYM";
                break;
            case DT_RELACOUNT:
                type = "RELACOUNT";
                break;
            case DT_RELCOUNT:
                type = "RELCOUNT";
                break;
            case DT_FLAGS_1:
                type = "FLAGS_1";
                break;
            case DT_VERDEF:
                type = "VERDEF";
                break;
            case DT_VERDEFNUM:
                type = "VERDEFNUM";
                break;
            case DT_VERNEED:
                type = "VERNEED";
                break;
            case DT_VERNEEDNUM:
                type = "VERNEEDNUM";
                break;
            case DT_AUXILIARY:
                type = "AUXILIARY";
                break;
            default:
                type = "";
                break;
        }

        sprintf(line + strlen(line), "(%s)%*.s", type, 18 - (int) strlen(type), " ");

        switch (my_dyn->d_tag) {
            case DT_NEEDED:
                sprintf(line + strlen(line), " Shared library: [%s]", (char *) my_elf->ehdr +
                    get_section(my_elf, ".dynstr")->sh_offset + my_dyn->d_un.d_val);
                break;
            case DT_INIT_ARRAYSZ:
            case DT_FINI_ARRAYSZ:
            case DT_STRSZ:
            case DT_PLTRELSZ:
            case DT_RELASZ:
            case DT_RELAENT:
                sprintf(line + strlen(line), " %ld (bytes)",
                    my_dyn->d_un.d_val);
                break;

            default:
                sprintf(line + strlen(line), " 0x%lx", my_dyn->d_un.d_val);
        }


        printf("%s\n", line);
        free(line);

        my_dyn++;
    }
}
