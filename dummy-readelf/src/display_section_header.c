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

void display_section_header(struct ELF *my_elf)
{
    printf("Section Headers:\n");
    ElfW(Shdr) *my_shdr = my_elf->shdr;

    printf("  [Nr] Name              Type             Address           Offset\n");
    printf("       Size              EntSize          Flags  Link  Info  Align\n");

    for (int i = 0; i < my_elf->ehdr->e_shnum; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "  [%.*s%d] ", (i < 10), " ", i);

        char *p = (char *) my_elf->ehdr;

        ElfW(Ehdr) *ehdr = (ElfW(Ehdr) *) p;
        ElfW(Shdr) *shdr = (ElfW(Shdr) *) (p + ehdr->e_shoff);
        ElfW(Shdr) *sh_strtab = &shdr[ehdr->e_shstrndx];
        const char *const sh_strtab_p = p + sh_strtab->sh_offset;

        sprintf(line + strlen(line), "%s%*.18s", sh_strtab_p + my_shdr->sh_name,
            18 - (int) strlen(sh_strtab_p + my_shdr->sh_name), "");

        char *type;
        switch (my_shdr->sh_type)
        {
            case SHT_NULL:
                type = "NULL";
                break;
            case SHT_PROGBITS:
                type = "PROGBITS";
                break;
            case SHT_SYMTAB:
                type = "SYMTAB";
                break;
            case SHT_STRTAB:
                type = "STRTAB";
                break;
            case SHT_RELA:
                type = "RELA";
                break;
            case SHT_HASH:
                type = "HASH";
                break;
            case SHT_DYNAMIC:
                type = "DYNAMIC";
                break;
            case SHT_NOTE:
                type = "NOTE";
                break;
            case SHT_NOBITS:
                type = "NOBITS";
                break;
            case SHT_REL:
                type = "REL";
                break;
            case SHT_SHLIB:
                type = "SHLIB";
                break;
            case SHT_DYNSYM:
                type = "DYNSYM";
                break;
            case SHT_LOPROC:
                type = "LOPROC";
                break;
            case SHT_HIPROC:
                type = "HIPROC";
                break;
            case SHT_LOUSER:
                type = "LOUSER";
                break;
            case SHT_HIUSER:
                type = "HIUSER";
                break;
            default:
                type = "";
                break;
        }

        sprintf(line + strlen(line), "%s%*.s", type, 15 - (int) strlen(type), " ");
        sprintf(line + strlen(line), "  %0*lx", 16, my_shdr->sh_addr);
        sprintf(line + strlen(line), "  %0*lx", 8, my_shdr->sh_offset);
        sprintf(line + strlen(line), "\n");

        //OFFSET
        sprintf(line + strlen(line), "       %0*lx", 16, my_shdr->sh_size);

        sprintf(line + strlen(line), "  %0*lx", 16, my_shdr->sh_entsize);


        char *flags = malloc(sizeof(char) * 8);
        if (my_shdr->sh_flags & SHF_WRITE)
            sprintf(flags + strlen(flags), "W");
        if (my_shdr->sh_flags & SHF_ALLOC)
            sprintf(flags + strlen(flags), "A");
        if (my_shdr->sh_flags & SHF_EXECINSTR)
            sprintf(flags + strlen(flags), "X");
        if (my_shdr->sh_flags & SHF_MASKPROC)
            sprintf(flags + strlen(flags), "MS");
        sprintf(line + strlen(line), "%*.s%s", 4 - (int) strlen(flags), " ", flags);
        free(flags);

        sprintf(line + strlen(line), "% 8d", my_shdr->sh_link);
        sprintf(line + strlen(line), " % 5d", my_shdr->sh_info);
        sprintf(line + strlen(line), " % 5ld", my_shdr->sh_addralign);

        printf("%s\n", line);

        free(line);

        char *data_str = (void *) my_shdr;
        unsigned offset = my_elf->ehdr->e_shentsize;
        my_shdr = (void *) &data_str[offset];
    }

    printf("Key to Flags:\n");
    printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
    printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
    printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
    printf("  l (large), p (processor specific)\n");


}
