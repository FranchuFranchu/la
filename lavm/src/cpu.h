#include <stdint.h>
#include <stdbool.h>

#include "memory.h"
#include "util_datatypes/hashmap.h"

#ifndef CPU_H
#define CPU_H

#define CLEAR_FLAG(cpu,shift) cpu->registers.flags &= ~(1 << shift)
#define SET_FLAG(cpu,shift) cpu->registers.flags |= (1 << shift)
#define GET_FLAG(cpu,shift) (cpu->registers.flags >> shift) & 1


struct registers
{
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
    uint64_t e;
    uint64_t f;
    uint64_t g;
    uint64_t h;
    uint64_t i;
    uint64_t flags;
    uint64_t unused0;
    uint64_t unused1;
    uint64_t unused2;
    uint64_t unused3;
    uint64_t unused4;
    uint64_t instruction_pointer;

};

struct debug_settings 
{
    bool log_loading;
    bool log_opcode_instructions;
    bool log_opcode_arguments;
    bool log_errors;
};

struct io_registers
{
    uint64_t stack_bottom;  
    uint64_t stack_current;
    uint64_t stack_top;
    uint64_t descriptor_table;
};

struct cpu
{
    struct registers registers;
    struct io_registers io_registers;
    struct memory memory;
    struct debug_settings debug_settings;
    uint64_t start_of_this_instruction;
    long i;
    char bits_mode;
    map_t libraries;
};

struct cpu * cpu_init(const char * filename);
void cpu_run(struct cpu * cpu);
void cpu_tick(struct cpu * cpu);

#endif // CPU_H
