#include <stdint.h>
#include <stdbool.h>

#include "memory.h"

#ifndef CPU_H
#define CPU_H

#define CLEAR_FLAG(cpu,shift) cpu->registers.flags &= ~(1 << shift)
#define SET_FLAG(cpu,shift) cpu->registers.flags |= (1 << shift)
#define GET_FLAG(cpu,shift) (cpu->registers.flags >> shift) & 1


struct registers
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;
    uint32_t i;
    uint32_t flags;
    uint32_t unused0;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t unused4;
    uint32_t instruction_pointer;

};

struct debug_settings 
{
    bool log_loading;
    bool log_opcode_instructions;
    bool log_opcode_arguments;
    bool log_errors;
};

struct cpu
{
    struct registers registers;
    struct memory memory;
    struct debug_settings debug_settings;
    long i;
};

struct cpu * cpu_init(char * filename);
void cpu_run(struct cpu * cpu);
void cpu_tick(struct cpu * cpu);

#endif // CPU_H
