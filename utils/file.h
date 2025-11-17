#pragma once
#include <cstdio>
#include <cstdlib>

inline char* file_read(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("Error opening file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = (char *)malloc(file_size + 1);
    fread(buffer, 1, file_size, file);

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}
