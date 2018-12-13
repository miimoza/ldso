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

static void c_display(char *field, char *value)
{
    printf("  %s%*s%s\n", field, 40 - (int) strlen(field), " ", value);
}

void display_elf_header(struct ELF *my_elf)
{
    printf("ELF Header:\n");
    char *buf = malloc(sizeof(char) * EI_NIDENT * 3);
    unsigned char *magic_number = &(my_elf->ehdr->e_ident[0]);
    for (unsigned i = 0; i < EI_NIDENT; i++)
        sprintf(buf + strlen(buf), "%02x ", magic_number[i]);

    sprintf(buf + strlen(buf), "%c", '\0');
    c_display("Magic:", buf);
    free(buf);

    switch (my_elf->ehdr->e_ident[EI_CLASS]) {
        case ELFCLASSNONE:
            c_display("Class:", "This class is invalid");
            break;
        case ELFCLASS32:
            c_display("Class:", "ELF32");
            break;
        case ELFCLASS64:
            c_display("Class:", "ELF64");
            break;
    }

    switch (my_elf->ehdr->e_ident[EI_DATA]) {
        case ELFDATANONE:
            c_display("Data:", "Unknown data format.");
            break;
        case ELFDATA2LSB:
            c_display("Data:", "2's complement, little-endian.");
            break;
        case ELFDATA2MSB:
            c_display("Data:", "2's complement, big-endian.");
            break;
    }

    switch (my_elf->ehdr->e_ident[EI_VERSION]) {
        case EV_NONE:
            c_display("Version:", "(0) invalid");
            break;
        case EV_CURRENT:
            c_display("Version:", "1 (current)");
            break;
    }

    switch (my_elf->ehdr->e_ident[EI_OSABI])
    {
        case ELFOSABI_SYSV || ELFOSABI_NONE:
            c_display("OS/ABI:", "UNIX - System V");
            break;
        case ELFOSABI_HPUX:
            c_display("OS/ABI:", "HP-UX ABI");
            break;
        case ELFOSABI_NETBSD:
            c_display("OS/ABI:", "NetBSD ABI");
            break;
        case ELFOSABI_LINUX:
            c_display("OS/ABI:", "Linux ABI");
            break;
        case ELFOSABI_SOLARIS:
            c_display("OS/ABI:", "Solaris ABI");
            break;
        case ELFOSABI_IRIX:
            c_display("OS/ABI:", "IRIX ABI");
            break;
        case ELFOSABI_FREEBSD:
            c_display("OS/ABI:", "FreeBSD ABI");
            break;
        case ELFOSABI_TRU64:
            c_display("OS/ABI:", "TRU64 UNIX ABI");
            break;
        case ELFOSABI_ARM:
            c_display("OS/ABI:", "ARM architecture ABI");
            break;
        case ELFOSABI_STANDALONE:
            c_display("OS/ABI:", "Stand-alone (embedded) ABI");
            break;
    }

    char *abiversion = malloc(sizeof(int));
    sprintf(abiversion, "%d", my_elf->ehdr->e_ident[EI_ABIVERSION]);
    c_display("ABI Version:", abiversion);
    free(abiversion);


    switch (my_elf->ehdr->e_type) {
        case ET_NONE:
            c_display("Type:", "unknown type");
            break;
        case ET_REL:
            c_display("Type:", "file");
            break;
        case ET_EXEC:
            c_display("Type:", "executable file");
            break;
        case ET_DYN:
            c_display("Type:", "DYN (Shared object file)");
            break;
        case ET_CORE:
            c_display("Type:", "core file");
            break;
    }

    switch (my_elf->ehdr->e_machine) {
        case EM_NONE:
            c_display("Machine:", "An unknown machine");
            break;
        case EM_M32:
            c_display("Machine:", "AT&T WE 32100");
            break;
        case EM_SPARC:
            c_display("Machine:", "Sun Microsystems SPARC");
            break;
        case EM_386:
            c_display("Machine:", "Intel 80386");
            break;
        case EM_68K:
            c_display("Machine:", "Motorola 68000");
            break;
        case EM_88K:
            c_display("Machine:", "Motorola 88000");
            break;
        case EM_860:
            c_display("Machine:", "Intel 80860");
            break;
        case EM_MIPS:
            c_display("Machine:", "MIPS RS3000 (big-endian only)");
            break;
        case EM_PARISC:
            c_display("Machine:", "HP/PA");
            break;
        case EM_SPARC32PLUS:
            c_display("Machine:", "SPARC with enhanced instruction set");
            break;
        case EM_PPC:
            c_display("Machine:", "PowerPC");
            break;
        case EM_PPC64:
            c_display("Machine:", "PowerPC 64-bit");
            break;
        case EM_S390:
            c_display("Machine:", "IBM S/390");
            break;
        case EM_ARM:
            c_display("Machine:", "Advanced RISC Machines");
            break;
        case EM_SH:
            c_display("Machine:", "Renesas SuperH");
            break;
        case EM_SPARCV9:
            c_display("Machine:", "SPARC v9 64-bit");
            break;
        case EM_IA_64:
            c_display("Machine:", "Intel Itanium");
            break;
        case EM_X86_64:
            c_display("Machine:", "Advanced Micro Devices X86-64");
            break;
        case EM_VAX:
            c_display("Machine:", "DEC Vax");
            break;
    }

    char *version = malloc(sizeof(char) * 16);
    sprintf(version, "0x%x", my_elf->ehdr->e_version);
    c_display("Version:", version);
    free(version);

    char *entry = malloc(sizeof(char) * 16);
    sprintf(entry, "0x%lx", my_elf->ehdr->e_entry);
    c_display("Entry point address:", entry);
    free(entry);

    char *phoff = malloc(sizeof(char) * 16);
    sprintf(phoff, "%ld (bytes into file)", my_elf->ehdr->e_phoff);
    c_display("Start of program headers:", phoff);
    free(phoff);

    char *shoff = malloc(sizeof(char) * 16);
    sprintf(shoff, "%ld (bytes into file)", my_elf->ehdr->e_shoff);
    c_display("Start of program headers:", shoff);
    free(shoff);

    c_display("Flags:", "0x0");

    char *ehsize = malloc(sizeof(int));
    sprintf(ehsize, "%d (bytes)", my_elf->ehdr->e_ehsize);
    c_display("Size of this header:", shoff);
    free(ehsize);

    char *phentsize = malloc(sizeof(int));
    sprintf(phentsize, "%d (bytes)", my_elf->ehdr->e_phentsize);
    c_display("Size of program headers:", phentsize);
    free(phentsize);

    char *phnum = malloc(sizeof(int));
    sprintf(phnum, "%d", my_elf->ehdr->e_phnum);
    c_display("Number of program headers: ", phnum);
    free(phnum);

    char *shentsize = malloc(sizeof(int));
    sprintf(shentsize, "%d (bytes)", my_elf->ehdr->e_shentsize);
    c_display("Size of section headers:", shentsize);
    free(shentsize);

    char *shnum = malloc(sizeof(int));
    sprintf(shnum, "%d", my_elf->ehdr->e_shnum);
    c_display("Number of section headers: ", shnum);
    free(shnum);

    char *shstrndx = malloc(sizeof(int));
    sprintf(shstrndx, "%d", my_elf->ehdr->e_shstrndx);
    c_display("Section header string table index:", shstrndx);
    free(shstrndx);
}
