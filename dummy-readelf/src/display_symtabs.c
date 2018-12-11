#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <link.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "read_elf.h"


void display_symtabs(struct ELF *my_elf)
{
    int nb_elt_dynsym = my_elf->shdr_dynsym->sh_size / my_elf->shdr_dynsym->sh_entsize;
    printf("Symbol table '.dynsym' contains %d entries:\n", nb_elt_dynsym);
    printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");

    ElfW(Sym) *my_dynsym = my_elf->dynsym;

    for (int i = 0; i < nb_elt_dynsym; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "    %2d:", i);
        sprintf(line + strlen(line), " %016lx%*.s", my_dynsym->st_value, 5, " ");
        sprintf(line + strlen(line), "%1ld ", my_dynsym->st_size);


        char *type;
        switch(ELFW(ST_TYPE)(my_dynsym->st_info))
        {
            case STT_NOTYPE:
                type = "NOTYPE";
                break;
            case STT_OBJECT:
                type = "OBJECT";
                break;
            case STT_FUNC:
                type = "FUNC";
                break;
            case STT_SECTION:
                type = "SECTION";
                break;
            case STT_FILE:
                type = "FILE";
                break;
            case STT_LOPROC:
            case STT_HIPROC:
                type = "STT PROC";
                break;
            default:
                type = "";
                break;
        }
        sprintf(line + strlen(line), "%s%*.s", type, 8 - (int) strlen(type), " ");

        switch(ELFW(ST_BIND)(my_dynsym->st_info))
        {
            case STB_LOCAL:
                type = "LOCAL";
                break;
            case STB_GLOBAL:
                type = "GLOBAL";
                break;
            case STB_WEAK:
                type = "WEAK";
                break;
            case STB_LOPROC:
            case STB_HIPROC:
                type = "STB PROC";
                break;
            default:
                type = "";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 7 - (int) strlen(type), " ");

        switch(ELFW(ST_VISIBILITY)(my_dynsym->st_other))
        {
            case STV_DEFAULT:
                type = "DEFAULT";
                break;
            case STV_INTERNAL:
                type = "INTERNAL";
                break;
            case STV_HIDDEN:
                type = "HIDDEN";
                break;
            case STV_PROTECTED:
                type = "PROTECTED";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 10 - (int) strlen(type), " ");

        sprintf(line + strlen(line), " %x", my_dynsym->st_shndx);


        printf("%s\n", line);
        free(line);

        my_dynsym++;
    }

    printf("\n");

    int nb_elt_symtab = my_elf->shdr_symtab->sh_size / my_elf->shdr_symtab->sh_entsize;
    printf("Symbol table '.symtab' contains %d entries:\n", nb_elt_symtab);
    printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");

    ElfW(Sym) *my_symtab = my_elf->symtab;
    for (int i = 0; i < nb_elt_symtab; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "    %2d:", i);
        sprintf(line + strlen(line), " %016lx", my_symtab->st_value);
        sprintf(line + strlen(line), "%6ld ", my_symtab->st_size);


        char *type;
        switch(ELFW(ST_TYPE)(my_symtab->st_info))
        {
            case STT_NOTYPE:
                type = "NOTYPE";
                break;
            case STT_OBJECT:
                type = "OBJECT";
                break;
            case STT_FUNC:
                type = "FUNC";
                break;
            case STT_SECTION:
                type = "SECTION";
                break;
            case STT_FILE:
                type = "FILE";
                break;
            case STT_LOPROC:
            case STT_HIPROC:
                type = "STT PROC";
                break;
            default:
                type = "";
                break;
        }
        sprintf(line + strlen(line), "%s%*.s", type, 8 - (int) strlen(type), " ");

        switch(ELFW(ST_BIND)(my_symtab->st_info))
        {
            case STB_LOCAL:
                type = "LOCAL";
                break;
            case STB_GLOBAL:
                type = "GLOBAL";
                break;
            case STB_WEAK:
                type = "WEAK";
                break;
            case STB_LOPROC:
            case STB_HIPROC:
                type = "STB PROC";
                break;
            default:
                type = "";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 7 - (int) strlen(type), " ");

        switch(ELFW(ST_VISIBILITY)(my_symtab->st_other))
        {
            case STV_DEFAULT:
                type = "DEFAULT";
                break;
            case STV_INTERNAL:
                type = "INTERNAL";
                break;
            case STV_HIDDEN:
                type = "HIDDEN";
                break;
            case STV_PROTECTED:
                type = "PROTECTED";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 9 - (int) strlen(type), " ");

        switch(my_symtab->st_shndx)
        {
            case SHN_UNDEF:
                sprintf(line + strlen(line), "UND");
                break;
            case SHN_ABS:
                sprintf(line + strlen(line), "ABS");
                break;
            default:
                sprintf(line + strlen(line), "%3d", my_symtab->st_shndx);
                break;
        }

        printf("%s\n", line);
        free(line);

        my_symtab++;
    }
}
