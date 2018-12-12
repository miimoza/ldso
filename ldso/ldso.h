#include "types.h"

void display_auxv(ElfW(auxv_t) *auxv);
ElfW(auxv_t) *get_auxv_entry(ElfW(auxv_t) *auxv, u32 type);
