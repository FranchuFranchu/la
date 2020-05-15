#include <stdint.h>
#include "cpu.h"

#ifndef CPU_STACK_H
#define CPU_STACK_H

uint64_t cpu_stack_pop(struct cpu * cpu);
void cpu_stack_push(struct cpu * cpu, uint64_t value);


#endif // CPU_STACK_H
