#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef MEMORY_H
#define MEMORY_H

struct memory_page
{
    uint8_t values[256];
};

/* 
Since the LAVM is a 64-bit machine
We need 64 bits to address a location
Each table provides 8 bits of precision
There are 8 layers of tables
so since 8*8 = 64
this is enough
*/

struct memory_page_table
{
    union {
        struct memory_page * subpages[256];
        struct memory_page_table * subtables[256];
    } children;
};

struct memory_debug_settings {
    bool log_reads;
    bool log_writes;
};

struct memory {
    struct memory_page_table table;
    struct memory_debug_settings debug_settings;
};

uint8_t * memory_get_location_of(struct memory * memory, uint64_t index);

void memory_set_char(struct memory * memory, uint64_t index, char c);
void memory_set_dword(struct memory * memory, uint64_t index, uint32_t c);
void memory_set_qword(struct memory * memory, uint64_t index, uint64_t c);

uint8_t memory_get_char(struct memory * memory, uint64_t index);
uint32_t memory_get_dword(struct memory * memory, uint64_t index);
uint64_t memory_get_qword(struct memory * memory, uint64_t index);

#endif // MEMORY_H
