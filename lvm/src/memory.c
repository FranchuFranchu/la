#include <stdlib.h>
#include "memory.h"

struct memory * memory_init()
{
    struct memory * memory = malloc(sizeof(memory));

    return memory;
};

void memory_set_char(struct memory * memory, int index, char c) {
    memory->values[index] = c;
}

char *  memory_get_char(struct memory * memory, int index) {
    return &memory->values[index];
}

uint32_t *  memory_get_dword(struct memory * memory, int index) {
    return (uint32_t*)&memory->values[index];
}