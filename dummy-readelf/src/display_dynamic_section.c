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
    printf("Dynamic section at offset 0x1d78 contains 27 entries:\n");
    printf("  Tag        Type                         Name/Value\n");

    ElfW(Dyn) *my_dyn = my_elf->dyn;

    for (int i = 0; i < 10; i++)
    {
        char *line = malloc(sizeof(char) * 256);
        sprintf(line + strlen(line), "  ");

        char *type;
        switch(my_dyn->d_tag)
        {
            case DT_NULL:
                type = "NULL";
                break;
            case DT_NEEDED:
                type = "NEEDED";
                break;
        }
        sprintf(line + strlen(line), "(%s)%*.s", type, 15 - (int) strlen(type), " ");



        sprintf(line + strlen(line), " 0x%0*lx", 16, my_dyn->d_tag);

        printf("%s\n", line);

        free(line);

        my_dyn++;
    }
}
