#include <stdint.h>

#include "cpu.h"

#ifndef CPU_OPERATION_H
#define CPU_OPERATION_H


enum operation {
    OPERATION_NONE = 0,
    OPERATION_ADD = 1,
    OPERATION_SUB = 2
};

uint64_t cpu_do_operation(struct cpu * cpu, uint64_t n1, uint64_t n2, enum operation operation);

#endif // CPU_OPERATION_H
