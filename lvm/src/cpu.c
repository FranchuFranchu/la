#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "memory.h"
#include "opcodes.h"
#include "operand_prefixes.h"

struct cpu * cpu_init(char * filename) 
{
    struct cpu * cpu = malloc(sizeof(struct cpu));

    // All registers initialized to zero
    memset(cpu, 0, sizeof(struct cpu));

    FILE * data_file = fopen(filename, "r");
    if (data_file == NULL) 
    {
        char dest[256];
        snprintf(dest, 256, "Error loading file %s", filename);
        perror(dest);
        return cpu;
    }

    // Copy file contents to memory to memory

    char c = 0;
    int index = 0;
    while(1)  
    {
        char c = fgetc(data_file);
        if (c == EOF)
        {
            printf("%s\n", "EOF");
            break;
        }
        printf("Loading %hhx at %d\n", c, index);
        memory_set_char(&cpu->memory, index, c);
        index++;
    }

    fclose(data_file);
    return cpu;
}

void cpu_run(struct cpu * cpu)
{
    while (1) 
    {
        cpu_tick(cpu);
        if (cpu->registers.flags.exit_to_system) {
            break;
        }
    }
}

int cpu_get_argument_count(int instruction) 
{
    return opcode_argcount[instruction];
}

int cpu_next_instruction(struct cpu * cpu) 
{
    int index = cpu->registers.instruction_pointer;
    printf("IP: %x\n", index);
    cpu->registers.instruction_pointer++;
    int r = *memory_get_char(&(cpu->memory), index);
    return r & 0xFF;
}

uint32_t * cpu_get_pointer_to_argument(struct cpu * cpu, char nibble) {
    if (nibble <= OPERAND_HIGHEST_REGISTER & nibble >= 0) 
    {
        uint32_t * reg = (uint32_t*)(&cpu->registers)+(nibble*4);
        
        return reg;
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORDARG) {

        int index = cpu->registers.instruction_pointer;
        cpu->registers.instruction_pointer+=4;
        uint32_t * value = memory_get_dword(&cpu->memory, index);
        if (nibble ==  OPERAND_SPECIAL_IMMEDIATE) 
        {  
            return value;
        }
        else 
        {
            printf("Unimplemented. TODO\n");
        }
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORD_CHAR) 
    {
        errno = 1;
        printf("Unimplemented. TODO.\n");
        exit(-1);
    }
    else 
    {
        printf("Invalid argument passed to cpu_get_pointer_to_argument.\n");
        exit(-1);
    }
}

void cpu_tick(struct cpu * cpu) 
{
    int instruction = cpu_next_instruction(cpu);
    int argcount = cpu_get_argument_count(instruction);
    printf("Instruction %x, %x arguments\n", instruction, argcount);

    uint32_t * operators[4] = {0,0,0,0};

    for (int i = 0; i < argcount / 2; ++i)
    {
    
        int index = cpu->registers.instruction_pointer;
        cpu->registers.instruction_pointer++;
        char asbyte = *memory_get_char(&cpu->memory, index);

        // lower nibble
        operators[i*2] = cpu_get_pointer_to_argument(cpu, asbyte & 0xF);
        // higher nibble
        operators[i*2+1] = cpu_get_pointer_to_argument(cpu,( asbyte & 0xF0) >> 4);

        printf(" Argument %d: %x, %x\n", i,  asbyte & 0xF, *operators[i*2]);
        printf(" Argument %d: %x, %x\n", i + 1, ( asbyte & 0xF0) >> 4, operators[i*2+1]);


    }

    switch(instruction) {
        case OPCODE_EXIT_TO_SYSTEM:
            cpu->registers.flags.exit_to_system = true;
            break;
        case OPCODE_MOV:
            *operators[1] = *operators[0];
            break;
        case OPCODE_DEBUG_DUMP_OPERATOR_0:
            printf("DEBUG: %x\n", *operators[0]);
            break;
    }

}