#include <stdint.h>
#include "cpu.h"

#ifndef CPU_STACK_H
#define CPU_STACK_H

uint32_t cpu_stack_pop(struct cpu * cpu);
void cpu_stack_push(struct cpu * cpu, uint32_t value);


#endif // CPU_STACK_H
