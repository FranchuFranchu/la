#include <stdint.h>

#include "cpu.h"

#ifndef IOPORT_C
#define IOPORT_C

enum registerlike_io_ports {
    IOPORT_STACK_BOTTOM = 0,
    IOPORT_STACK_CURRENT = 1,
    IOPORT_STACK_TOP = 2,
    IOPORT_DESCRIPTOR_TABLE = 3,
};

#define IOPORT_MAX_REGISTERLIKE IOPORT_DESCRIPTOR_TABLE

uint32_t ioport_in(struct cpu * cpu, uint32_t port);
void ioport_out(struct cpu * cpu, uint32_t port, uint32_t value);

#endif // IOPORT_C
