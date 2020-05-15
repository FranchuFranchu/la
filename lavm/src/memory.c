#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "logging.h"

struct memory * memory_init()
{
    struct memory * memory = malloc(sizeof(struct memory));

    memset(memory, 0, sizeof(struct memory));

    return memory;
};

uint8_t * memory_get_location_of(struct memory * memory, uint64_t index) 
{
    struct memory_page_table * current_table = &memory->table;
    struct memory_page_table * prev_table = NULL;
    struct memory_page * current_page = NULL;



    uint64_t shifted_index = index;
    for (int i = 0; i < 7; i++)
    {
        // Take the highest byte
        uint8_t this_index = (shifted_index & (0xFFUL << (64-8))) >> (64-8);
        if (i == 6) {
            // last layer; use subpages instead of subtables
            prev_table = current_table;
            current_page = current_table->children.subpages[this_index];

            if (current_page == NULL)  {
                prev_table = current_table;
                current_page = malloc(sizeof(struct memory_page));
                memset(current_page, 0, sizeof(struct memory_page));
                prev_table->children.subpages[this_index] = current_page;

            }

        } else {
            prev_table = current_table;
            current_table = current_table->children.subtables[this_index];

            if (current_table == NULL)  {
                current_table = malloc(sizeof(struct memory_page_table));
                memset(current_table, 0, sizeof(struct memory_page_table));
                prev_table->children.subtables[this_index] = current_table;
            }

        }


        // Put the next index in the highest position
        shifted_index <<= 8;


        current_table = current_table;
    }


    // Take the highest byte
    uint8_t this_index = (shifted_index & (0xFFUL << (64-8))) >> (64-8);
    uint8_t * value = &(current_page->values[this_index]);
    return value;

}

// Word size must be multiple of 8
void memory_set_size(struct memory * memory, uint64_t index, uint64_t data_v, char word_size)
{
    LOG_MEMORY_READ("[WRITE] 0x%lx = 0x%lx\n", index, data_v);
    if (word_size % 8 != 0) {
        printf("Error: word_size must be multiple of 8!");
        exit(-1);
    }   
    // Now, set each byte
    // The LAVM is little endian (least significant first)
    for (char i = 0; i < (word_size >> 3); i++)
    {
        *memory_get_location_of(memory, index + i) = (data_v >> i * 8) & 0xFF;
    }
} 


uint64_t memory_get_size(struct memory * memory, uint64_t index, char word_size)
{
    if (word_size % 8 != 0) {
        printf("Error: word_size must be multiple of 8!");
        exit(-1);
    }
    uint64_t result = 0;
    for (char i = 0; i < (word_size >> 3); i++)
    {
        result |= ((uint64_t)(*memory_get_location_of(memory, index + i)) << (i * 8));
    }
    LOG_MEMORY_WRITE("[READ ] 0x%lx to 0x%lx = 0x%lx\n", index, index + word_size / 8, result);
    return result;
} 


void memory_set_char(struct memory * memory, uint64_t index, char c) 
{
    memory_set_size(memory, index, c, 8);
}
void memory_set_dword(struct memory * memory, uint64_t index, uint32_t c) 
{
    memory_set_size(memory, index, c, 32);
}
void memory_set_qword(struct memory * memory, uint64_t index, uint64_t c) 
{
    memory_set_size(memory, index, c, 64);
}

uint8_t memory_get_char(struct memory * memory, uint64_t index) 
{
    return memory_get_size(memory, index, 8);
}
uint32_t memory_get_dword(struct memory * memory, uint64_t index) 
{
    return memory_get_size(memory, index, 32);
}
uint64_t memory_get_qword(struct memory * memory, uint64_t index)
{
    return memory_get_size(memory, index, 64);
}
