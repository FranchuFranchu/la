#include "cpu_stack.h"
#include "memory.h"

uint32_t cpu_stack_pop(struct cpu * cpu) {
    cpu->io_registers.stack_current += 4;
    return *memory_get_dword(&cpu->memory, cpu->io_registers.stack_current);
}

void cpu_stack_push(struct cpu * cpu, uint32_t value) {
    memory_set_dword(&cpu->memory, cpu->io_registers.stack_current, value);
    cpu->io_registers.stack_current -= 4;
}