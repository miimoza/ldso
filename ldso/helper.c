#include "ldso.h"

int my_var_cmp(char *a, char *b)
{
    int i = 0;
    while (a[i] != '=' && b[i] != '=' &&
        a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
        i++;
    return a[i] == b[i];
}

int my_str_cmp(char *a, char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
        i++;
    return a[i] == b[i];
}
