#include "ldso.h"
#include "string.h"

char *get_name_from_path(char *path)
{
    int i = strlen(path) - 1;
    while (path[i] != '\0' && path[i] != '/')
        i--;
    return path + i + 1;
}

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
