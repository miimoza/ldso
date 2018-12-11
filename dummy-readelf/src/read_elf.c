#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <link.h>
#include <elf.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#include "read_elf.h"

struct ELF *loader(char *path)
{
    struct ELF *my_elf = malloc(sizeof(struct ELF));
    int fd = open(path, O_RDONLY);
    struct stat stat_buffer;
    fstat(fd, &stat_buffer);
    my_elf->ehdr = mmap(0, stat_buffer.st_size,
        PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    char *data_str = (void *) my_elf->ehdr;
    unsigned sh_offset = my_elf->ehdr->e_shoff;
    my_elf->shdr = (void *) &data_str[sh_offset];

    unsigned ph_offset = my_elf->ehdr->e_phoff;
    my_elf->phdr = (void *) &data_str[ph_offset];

    int i = 0;

    ElfW(Shdr) *section = my_elf->shdr;
    while(i < my_elf->ehdr->e_shnum)
    {
        if (section->sh_type == SHT_DYNAMIC)
        {
            my_elf->dyn = (void *) &data_str[section->sh_offset];
            my_elf->shdr_dyn = section;
        }
        if (section->sh_type == SHT_DYNSYM)
        {
            my_elf->dynsym = (void *) &data_str[section->sh_offset];
            my_elf->shdr_dynsym = section;
        }
        if (section->sh_type == SHT_SYMTAB)
        {
            my_elf->symtab = (void *) &data_str[section->sh_offset];
            my_elf->shdr_symtab = section;
        }
        char *c_section = (void *) section;
        unsigned offset = my_elf->ehdr->e_shentsize;
        section = (void *) &c_section[offset];
        i++;
    }

    return my_elf;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        errx(1, "usage: %s <ELF>", argv[0]);

    struct ELF *my_elf = loader(argv[1]);

    display_elf_header(my_elf);
    printf("\n");
    display_section_header(my_elf);
    printf("\n");
    display_program_header(my_elf);
    /*printf("\n");
    display_dynamic_section(my_elf);
    printf("\n");
    display_symtabs(my_elf);
    */
    free(my_elf);
    return 0;
}
