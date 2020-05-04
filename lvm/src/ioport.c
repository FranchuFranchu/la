#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "ioport.h"


uint32_t ioport_in(struct cpu * cpu, uint32_t port) 
{
    if (port <= IOPORT_MAX_REGISTERLIKE) {
        return *(uint32_t*)(&cpu->io_registers + port * 4);
    }
    return 0;
}
void ioport_out(struct cpu * cpu, uint32_t port, uint32_t value)
{
    printf("%d %d\n", value, port);
    if (port <= IOPORT_MAX_REGISTERLIKE) {
        *(uint32_t*)(&cpu->io_registers + port * 4) = value;
        return;
    }

}
