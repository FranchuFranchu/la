#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "cpu_operation.h"
#include "flags.h"
#include "memory.h"
#include "logging.h"
#include "opcodes.h"
#include "operand_prefixes.h"

struct cpu * cpu_init(char * filename) 
{
    struct cpu * cpu = malloc(sizeof(struct cpu));

    // All registers initialized to zero
    memset(cpu, 0, sizeof(struct cpu));

    //cpu->debug_settings.log_opcode_arguments = true;
    //cpu->debug_settings.log_opcode_instructions = true;



    FILE * data_file = fopen(filename, "r");
    if (data_file == NULL) 
    {
        char dest[256];
        snprintf(dest, 256, "Error loading file %s", filename);
        perror(dest);
        return cpu;
    }

    // Copy file contents to memory to memory

    int c = 0;
    int index = 0;
    while(1)  
    {
        int c = fgetc(data_file);
        if (c == EOF)
        {
            break;
        }
        if (cpu->debug_settings.log_loading) {
            printf("Loading %hhx at %d\n", c, index);
        }
        memory_set_char(&cpu->memory, index, c & 0xFF);
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
        if (GET_FLAG(cpu,FLAG_EXIT)) {
            printf("Flag-based exit. Abort.\n");
            break;
        }
    }
}

int cpu_get_argument_count(int instruction) 
{
    // Unimplemented
    //return opcode_argcount[instruction];
}

int cpu_next_instruction(struct cpu * cpu) 
{
    int index = cpu->registers.instruction_pointer;
    cpu->registers.instruction_pointer++;
    int r = *memory_get_char(&(cpu->memory), index);
    return r & 0xFF;
}

uint32_t * cpu_get_pointer_to_argument(struct cpu * cpu, char nibble) {
    if (nibble == OPERAND_REGISTER_FLAGS) {

        uint32_t * reg = &cpu->registers.flags;
        
        return reg;   
    }
    else if (nibble <= OPERAND_HIGHEST_REGISTER & nibble >= 0) 
    {
        uint32_t * reg = (uint32_t*)(&cpu->registers)+(nibble*4);
        
        return reg;
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORDARG) 
    {
        int index = cpu->registers.instruction_pointer;
        cpu->registers.instruction_pointer+=4;
        uint32_t * value = memory_get_dword(&cpu->memory, index);
        if (nibble ==  OPERAND_SPECIAL_IMMEDIATE) 
        {  
            return value;
        }
        else 
        {
            printf("Unimplemented. TODO. (1)\n");
        }
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORD_CHAR) 
    {
        errno = 1;
        printf("Unimplemented. TODO. (2)\n");
        exit(-1);
    }
    else 
    {
        printf("Invalid argument passed to cpu_get_pointer_to_argument.\n");
        exit(-1);
    }
}

uint32_t * cpu_parse_argument(struct cpu * cpu, char type_nibble, char expected_argument_type) {
    uint32_t * ret;

    switch(expected_argument_type) {
        case 1:;
            ret = cpu_get_pointer_to_argument(cpu, type_nibble);
            break;
        case 2:;
            int index = cpu->registers.instruction_pointer;
            cpu->registers.instruction_pointer++;
            char b = *memory_get_char(&cpu->memory, index);
            ret = malloc(4);
            *ret = b;
            *ret = *ret & 0xFF;
            break;
        default:;
            LOG_ERROR("Error: \n");
            ret = malloc(4);



    }
    LOG_OPCODE_ARGUMENT(" Argument: Expect: %d, Type: 0x%x, Value: %x\n", expected_argument_type, type_nibble, *ret);
    return ret;

}

void cpu_tick(struct cpu * cpu) 
{  
    int index = cpu->registers.instruction_pointer;
    int instruction = cpu_next_instruction(cpu);
    cpu->i++;
    uint32_t * operators[4] = {0,0,0,0};

    LOG_OPCODE_INSTRUCTION("Instruction %x at 0x%x:\n", instruction, index);

    for (int i = 0; i < 2; ++i)
    {
        if (opcode_args[instruction][i*2] == 0) {
            break;
        }
        int index = cpu->registers.instruction_pointer;

        // lower nibble

        char expected_argument_type = opcode_args[instruction][i*2];

        char asbyte = *memory_get_char(&cpu->memory, index);
        if (expected_argument_type == 1) {
            cpu->registers.instruction_pointer++;
        }
        operators[i*2] = cpu_parse_argument(cpu, asbyte & 0xF, expected_argument_type);

        // higher nibble

        if (opcode_args[instruction][i*2+1] == 0) {
            break;
        }

        expected_argument_type = opcode_args[instruction][i*2+1];
        operators[i*2+1] = cpu_parse_argument(cpu, (asbyte >> 4) & 0xF, expected_argument_type);
        

    }
    uint32_t result;
    switch(instruction) {
        case OPCODE_EXIT_TO_SYSTEM:
            SET_FLAG(cpu,FLAG_EXIT);
            break;
        case OPCODE_MOV:
            *operators[1] = *operators[0];
            break;
        case OPCODE_DEBUG_DUMP_OPERATOR_0:
            printf("DEBUG: 0x%x = %d\n", *operators[0], *operators[0]);
            break;
        case OPCODE_JUMP:
            cpu->registers.instruction_pointer = *operators[0];
            break;
        case OPCODE_JUMP_CONDITION:;
            char shift_value = *operators[1] & 0x7F;
            bool flag_on = cpu->registers.flags >> shift_value & 1;
            bool invert = *operators[1] & 0x80;
            if (flag_on ^ invert) {
                cpu->registers.instruction_pointer = *operators[0];;
            }
            break;
        case OPCODE_BINARY_OPERATION:;
            result = cpu_do_operation(cpu, *operators[0], *operators[1], *operators[2]);
            *operators[1] = result;

        case OPCODE_BINARY_OPERATION_NOCHANGE:;
            result = cpu_do_operation(cpu, *operators[0], *operators[1], *operators[2]);

    }

}