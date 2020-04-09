#include <stdint.h>
#include "memory.h"
#include <stdbool.h>

#ifndef CPU_H
#define CPU_H


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
    union {
        uint32_t asint;
        bool exit_to_system;
    } flags;
    uint32_t unused0;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t unused4;
    uint32_t instruction_pointer;

};

struct cpu
{
    struct registers registers;
    struct memory memory;
};

struct cpu * cpu_init(char * filename);
void cpu_run(struct cpu * cpu);
void cpu_tick(struct cpu * cpu);

#endif // CPU_H
