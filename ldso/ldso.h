#ifndef LDSO_H
#define LDSO_H

#include <elf.h>
#include <link.h>

/* Macro Function to have the good size (32 or 64) with the upper case format
ie: ELF64_R_SYM(val) */
#define ELFW(type)	_ELFW (ELF, __ELF_NATIVE_CLASS, type)
#define _ELFW(e,w,t)	_ELFW_1 (e, w, _##t)
#define _ELFW_1(e,w,t)	e##w##t

#include "types.h"


enum env_vars
{
    VAR_LD_SHOW_AUXV = 1,
    VAR_LD_TRACE_LOADED_OBJECTS
};

struct path_list
{
    char *pathname;
    struct path_list *next;
};

struct Context
{
    ElfW(auxv_t) *auxv;
    int env_var_display;
    struct path_list *library_path_list;
    struct ELF *bin;
    struct link_map *link_map;
    struct link_map *library_link_map;
};

struct ELF
{
    char *pathname;
    ElfW(Ehdr) *ehdr;
    ElfW(Shdr) *shdr;
    ElfW(Phdr) *phdr;
    ElfW(Dyn) *dyn;
    ElfW(Shdr) *shdr_dyn;
    ElfW(Sym) *dynsym;
    ElfW(Shdr) *shdr_dynsym;
    ElfW(Sym) *symtab;
    ElfW(Shdr) *shdr_symtab;
    ElfW(Sym) *dynstr;
    ElfW(Shdr) *shdr_dynstr;
};


/*============================ MAIN FUNCTIONS ===============================*/

/* Build the link map in a recursive way, get the binary, the interpreter ldso,
the vdso, and all the libraries needed */
struct link_map *build_link_map(struct Context *my_context,
    struct ELF *my_elf, struct path_list *library_path);

/* Apply all the relocations for the given context, containing the whole
link map */
void apply_relocations(struct Context *my_context);

/*=========================== LOADER AND MAPPER =============================*/

/* Load the LOAD segments of the elf given, mmap with good persmissions */
void *dso_loader(struct ELF *my_elf, char *pathname);

/* Load the file given (pathname), mmap, and fill the struct ELF */
struct ELF *elf_loader(char *pathname, void *addr);

/*================================ DISPLAY ==================================*/

/* Display the auxiliar vector (when LD_SHOW_AUXV is in the environnement)*/
void display_auxv(ElfW(auxv_t) *auxv);

/*================================== FREE ===================================*/

/* Free the context structure and all objects allocated
which depends of the context (link map,..) */
void free_context(struct Context *my_context);

/*================================= HELPER ==================================*/

/* Get the name of a file from the full path
ie : get_name_from_path("~/Docs/me/foo") -> "foo" */
char *get_name_from_path(char *path);

/* compare two string with an environnement behaviour : the strings should be
the same just before the first '=', don't care what is after. If they are
the same return 1, else 0 */
int my_var_cmp(char *a, char *b);

/* Compare two strings, return 1 if they are exactly the same, else 0 */
int my_str_cmp(char *a, char *b);

/*=============================== PATH LISTS ================================*/

/* Fill the path_list structure with the string,
each path being separate by : */
struct path_list *build_library_path_list(char *lib_path_var);

/* Create a node of type path_list, filling with the value pathname given
in the structure */
struct path_list *create_path_node(char *pathname, int len);

/*================================== UTILS ==================================*/

/* Return number of elements in the dynamic table */
int get_dyn_num(ElfW(Dyn) *my_dyn);

/* Return the name of the section from the address of the section header. */
char *get_section_name(struct ELF *my_elf, ElfW(Shdr) *section);

/* Return the first sub section called <name> in .dynamic section,
if nothing is found, return the last section */
ElfW(Dyn) *get_dyn_section(struct ELF *my_elf , int tag);

/* Return the first section called <name>, if nothing is found,
return the last section */
ElfW(Shdr) *get_section(struct ELF *my_elf, char *name);

/* Return the entry corresponding to the variable type in the auxiliar vector*/
ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type);

#endif /* LDSO_H */
