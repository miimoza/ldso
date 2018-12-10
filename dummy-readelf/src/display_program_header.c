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

void display_program_header(struct ELF *my_elf)
{
    printf("Program Headers:\n");
    printf("  Type           Offset             VirtAddr           PhysAddr\n");
    printf("                 FileSiz            MemSiz              Flags  Align\n");

    ElfW(Phdr) *my_phdr = my_elf->phdr;

    for (int i = 0; i < my_elf->ehdr->e_phnum; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "  ");

        char *type;
        switch(my_phdr->p_type)
        {
            case PT_NULL:
                type = "NULL";
                break;
            case PT_LOAD:
                type = "LOAD";
                break;
            case PT_DYNAMIC:
                type = "DYNAMIC";
                break;
            case PT_INTERP:
                type = "INTERP";
                break;
            case PT_NOTE:
                type = "NOTE";
                break;
            case PT_SHLIB:
                type = "SHLIB";
                break;
            case PT_PHDR:
                type = "PHDR";
                break;
            case PT_LOPROC:
            case PT_HIPROC:
                type = "LOPROC";
                break;
            case PT_GNU_STACK:
                type = "GNU_STACK";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 15 - (int) strlen(type), " ");

        sprintf(line + strlen(line), "0x%0*lx", 16, my_phdr->p_offset);
        sprintf(line + strlen(line), " 0x%0*lx", 16, my_phdr->p_vaddr);
        sprintf(line + strlen(line), " 0x%0*lx\n", 16, my_phdr->p_paddr);
        sprintf(line + strlen(line), "%*.s0x%0*lx", 17, " ", 16, my_phdr->p_filesz);
        sprintf(line + strlen(line), " 0x%0*lx", 16, my_phdr->p_memsz);

        char *flags = malloc(sizeof(char) * 8);
        if (my_phdr->p_flags & PF_R)
            sprintf(flags + strlen(flags), "R");
        if (my_phdr->p_flags & PF_W)
            sprintf(flags + strlen(flags), "W");
        if (my_phdr->p_flags & PF_X)
            sprintf(flags + strlen(flags), "E");
        sprintf(line + strlen(line), "  %s%*.s", flags, 6 - (int) strlen(flags), " ");
        free(flags);

        sprintf(line + strlen(line), " 0x%lx", my_phdr->p_align);

        printf("%s\n", line);

        free(line);

        char *data_str = (void *) my_phdr;
        unsigned offset = my_elf->ehdr->e_phentsize;
        my_phdr = (void *) &data_str[offset];
    }
}
