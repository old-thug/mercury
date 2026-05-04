
#ifndef FS_H
#define FS_H

#ifdef _WIN32
#    error "Windows not yet supported"
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>

#include "./def.h"

inlined
const char *fs_canonicalpath(const char *path)
{
    char buffer[PATH_MAX] = {0};
    const char *res = realpath(path, buffer);
    return strdup(res);
}

inlined
bool fs_exists(const char *path)
{
    return access(path, F_OK) == 0;
}

inlined
ssize fs_read_entire_file(const char *path, char **out)
{
    FILE *file = fopen(path, "rb");
    if (!file) {
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    usize size = ftell(file);
    rewind(file);
    *out = arr_reserve(*out, size + 1);

    int ch = 0;
    
    while ((ch = fgetc(file)) != EOF) {
        arr_push(*out, ch);
    }
    arr_push(*out, 0);

    fclose(file);
    return size;
}

#endif /* FS_H */
