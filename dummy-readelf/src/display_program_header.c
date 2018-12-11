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

    my_phdr = my_elf->phdr;

    printf("\n Section to Segment mapping:\n");
    printf("  Segment Sections...\n");

    char *p = (char *) my_elf->ehdr;
    ElfW(Shdr) *shdr = (ElfW(Shdr) *) (p + my_elf->ehdr->e_shoff);
    ElfW(Shdr) *sh_strtab = &shdr[my_elf->ehdr->e_shstrndx];
    const char *const sh_strtab_p = p + sh_strtab->sh_offset;
    ElfW(Shdr) *my_shdr = my_elf->shdr;

    char *data_str = (void *) my_phdr;
    void *addr_next_phdr = data_str + my_elf->ehdr->e_phentsize;

    for (int i = 0; i < my_elf->ehdr->e_phnum; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "   %02d     ", i);

        void *addr_phdr = (void *) my_phdr->p_offset;
        void *addr_shdr = (void *) my_shdr->sh_offset;
        printf("new section\n");
        printf("%p in [%p; %p] ?\n", addr_shdr, addr_phdr, addr_next_phdr);
        while (addr_shdr > addr_phdr && addr_shdr < addr_next_phdr)
        {

            printf("%p in [%p; %p] ?\n", addr_shdr, addr_phdr, addr_next_phdr);

            printf("hey%s\n", sh_strtab_p + my_shdr->sh_name);
            sprintf(line + strlen(line), "%s%*.18s", sh_strtab_p + my_shdr->sh_name,
                18 - (int) strlen(sh_strtab_p + my_shdr->sh_name), "");
            char *str_shdr = (void *) my_shdr;
            my_shdr = (void *) &str_shdr[my_elf->ehdr->e_shentsize];
        }

        printf("%s\n", line);
        free(line);

        char *data_str = (void *) my_phdr;
        ElfW(Phdr) *next_phdr = (void *) (data_str + (my_elf->ehdr->e_phentsize * 2));
        addr_next_phdr = (void *) next_phdr->p_offset;
        my_phdr = (void *) (data_str + my_elf->ehdr->e_phentsize);
    }
}
