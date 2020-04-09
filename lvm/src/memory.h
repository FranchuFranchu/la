#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

struct memory_subpage
{
    uint8_t values[4096];
};

struct memory_superpage
{
    uint32_t pages[1024];
};

struct memory
{
    uint8_t values[4096];
};

void memory_set_char(struct memory * memory, int index, char c);
char * memory_get_char(struct memory * memory, int index);
uint32_t * memory_get_dword(struct memory * memory, int index);

#endif // MEMORY_H
