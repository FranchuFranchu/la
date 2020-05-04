#include <stdlib.h>
#include "memory.h"

struct memory * memory_init()
{
    struct memory * memory = malloc(sizeof(memory));

    return memory;
};

void memory_set_char(struct memory * memory, uint32_t index, char c) {
    struct memory_superpage * superpage;
    if ((superpage = memory->superpages[index >> 22 & 0x3ff]) == NULL) {
        // Allocate superpage
        superpage = malloc(sizeof(struct memory_superpage));
        memset(superpage, 0, sizeof(struct memory_superpage));
        memory->superpages[index >> 22 & 0x3ff] = superpage;

    }
    struct memory_subpage * subpage;

    if ((subpage = superpage->subpages[index >> 12 & 0x3ff]) == NULL) {
        // Allocate subpage
        subpage = malloc(sizeof(struct memory_subpage));
        memset(subpage, 0, sizeof(struct memory_subpage));
        superpage->subpages[index >> 12 & 0x3ff] = subpage;
    }

    subpage->values[index & 0x3ff] = c;
}

uint8_t *  memory_get_char(struct memory * memory, uint32_t index) {
    struct memory_superpage * superpage;
    if ((superpage = memory->superpages[index >> 22 & 0x3ff]) == NULL) {
        // If page not allocated it means the value is 0
        return 0;

    }
    struct memory_subpage * subpage;

    if ((subpage = superpage->subpages[index >> 12 & 0x3ff]) == NULL) {
        
        // If page not allocated it means the value is 0
        return 0;
    }

    return &subpage->values[index & 0x3ff];
}

uint32_t *  memory_get_dword(struct memory * memory, uint32_t index) {

    return (uint32_t*)memory_get_char(memory, index);
}