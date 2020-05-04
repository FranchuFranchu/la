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
    struct memory_subpage * subpages[1024];
};

struct memory
{
    struct memory_superpage * superpages[1024];
};

void memory_set_char(struct memory * memory, uint32_t index, char c);
uint8_t * memory_get_char(struct memory * memory, uint32_t index);
uint32_t * memory_get_dword(struct memory * memory, uint32_t index);

#endif // MEMORY_H
