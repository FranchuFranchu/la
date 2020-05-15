#include <stdint.h>
#include <stdio.h>

#include "cpu_operation.h"
#include "flags.h"

uint64_t cpu_do_operation(struct cpu * cpu, uint64_t n1, uint64_t n2, enum operation operation) {
    long result;
    switch(operation) {
        case OPERATION_NONE:;
            break;
        case OPERATION_ADD:;
            result = n1 + n2;
            if (result > UINT32_MAX) {
                SET_FLAG(cpu, FLAG_OVERFLOW);
            }
            break;
        case OPERATION_SUB:;
            result = n1 - n2;
            if (result < 0) {
                SET_FLAG(cpu,FLAG_UNDERFLOW);
                SET_FLAG(cpu,FLAG_LESS);
            } else if (result > 0) {
                SET_FLAG(cpu,FLAG_GREATER);
            } else {
                SET_FLAG(cpu,FLAG_ZERO);
            }
            break;

    }
    return result & 0xFFFFFFFFFFFFFFFF;
}
