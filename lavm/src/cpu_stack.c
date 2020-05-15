#include "cpu_stack.h"
#include "memory.h"

uint64_t cpu_stack_pop(struct cpu * cpu) 
{   
    cpu->io_registers.stack_current += 8;
    return memory_get_qword(&cpu->memory, cpu->io_registers.stack_current);
}

void cpu_stack_push(struct cpu * cpu, uint64_t value) 
{
    memory_set_qword(&cpu->memory, cpu->io_registers.stack_current, value);
    cpu->io_registers.stack_current -= 8;
}