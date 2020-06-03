#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ltdl.h>
#include <string.h>

#include "cpu.h"
#include "cpu_operation.h"
#include "cpu_stack.h"
#include "elf_file_parsing.h"
#include "flags.h"
#include "memory.h"
#include "logging.h"
#include "opcodes.h"
#include "operand_prefixes.h"
#include "ioport.h"
#include "system_calls/entry_point.h"

struct cpu * cpu_init(const char * filename) 
{
    struct cpu * cpu = malloc(sizeof(struct cpu));

    // All registers initialized to zero
    memset(cpu, 0, sizeof(struct cpu));

    cpu->libraries = hashmap_new();


    cpu->debug_settings.log_opcode_instructions = true;
    cpu->debug_settings.log_opcode_arguments = true;

    // cpu->memory.debug_settings.log_writes = true;
    // cpu->memory.debug_settings.log_reads = true;

    cpu->io_registers.stack_current = 0UL-8;

    FILE * data_file = fopen(filename, "r");
    if (data_file == NULL) 
    {
        char dest[256];
        snprintf(dest, 256, "Error loading file %s", filename);
        perror(dest);
        exit(-1);
    }

    elf_load_file(&cpu->memory, data_file);

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

int cpu_next_instruction(struct cpu * cpu) 
{
    uint64_t index = cpu->registers.instruction_pointer;
    cpu->registers.instruction_pointer++;
    uint8_t r = memory_get_char(&cpu->memory, index);
    return r;
}

uint64_t cpu_get_value_of_argument(struct cpu * cpu, char nibble, uint64_t * operand_extra_loc) {
    if (nibble == OPERAND_REGISTER_FLAGS) {

        uint64_t reg = cpu->registers.flags;
        
        return reg;   
    }
    else if ((nibble <= OPERAND_HIGHEST_REGISTER) && (nibble >= 0)) 
    {
        uint64_t * reg_ptr = ((uint64_t*)(&cpu->registers)+((nibble - 1)));
        uint64_t reg = *reg_ptr;
        
        return reg;
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORDARG) 
    {
        uint64_t index = cpu->registers.instruction_pointer;
        *operand_extra_loc = index;
        cpu->registers.instruction_pointer+=4;
        uint64_t provided_argument = memory_get_dword(&cpu->memory, index);
        if (nibble ==  OPERAND_SPECIAL_IMMEDIATE) 
        {  
            return provided_argument;
        }
        else if (nibble == OPERAND_MEMORY_ABSOLUTE) 
        {

            return  memory_get_qword(&cpu->memory, provided_argument);
        }
        else if (nibble == OPERAND_MEMORY_RELATIVE) 
        {
            // It's not a problem if it overflows
            uint64_t mem_location = cpu->start_of_this_instruction + provided_argument;
            return  memory_get_qword(&cpu->memory, mem_location);


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
    printf("Invalid argument passed to cpu_get_value_of_argument: %hhx.\n", nibble);
    exit(-1);
}


uint64_t cpu_parse_argument(struct cpu * cpu, char type_nibble, char expected_argument_type, uint64_t * operand_extra_loc) {
    uint64_t ret;

    switch(expected_argument_type) {
        case 1:;
            ret = cpu_get_value_of_argument(cpu, type_nibble, operand_extra_loc);
            break;
        case 2:;
            uint64_t index = cpu->registers.instruction_pointer;
            cpu->registers.instruction_pointer++;
            ret = memory_get_char(&cpu->memory, index);
            break;
        default:;
            LOG_ERROR("Error: Wrong expected type\n");
            exit(-1);



    }
    LOG_OPCODE_ARGUMENT(" Argument: Expect: %d, Type: 0x%x, Value: %lx\n", expected_argument_type, type_nibble, ret);
    return ret;

}

void cpu_set_operand(struct cpu * cpu,  uint8_t nibble, uint64_t operand_extra_loc, uint64_t value) 
{
    if (nibble == OPERAND_REGISTER_FLAGS) {

        cpu->registers.flags = value;
        return;
    }
    else if (nibble <= OPERAND_HIGHEST_REGISTER)
    {
        uint64_t * reg_ptr = ((uint64_t*)&cpu->registers)+((nibble - 1));
        *reg_ptr = value;
        return;
    } 
    else if (nibble <= OPERAND_HIGHEST_DWORDARG) 
    {
        uint64_t provided_argument = memory_get_dword(&cpu->memory, operand_extra_loc);
        if (nibble ==  OPERAND_SPECIAL_IMMEDIATE) 
        {  
            return;
        }
        else if (nibble == OPERAND_MEMORY_ABSOLUTE) 
        {
            memory_set_dword(&cpu->memory, operand_extra_loc, provided_argument);
            return;
        }
        else if (nibble == OPERAND_MEMORY_RELATIVE) 
        {
            // It's not a problem if it overflows
            uint64_t mem_location = cpu->start_of_this_instruction + provided_argument;
            memory_set_dword(&cpu->memory, mem_location, provided_argument);


            return;
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
    printf("Invalid argument passed to cpu_set_operand: %hhx\n", nibble);
    exit(-1);
    
    
}

void cpu_tick(struct cpu * cpu) 
{  
    uint64_t index = cpu->registers.instruction_pointer;
    uint8_t instruction = cpu_next_instruction(cpu);
    cpu->i++;
    // Allocate space for the 4 operators

    uint64_t * operand_values = calloc(4, sizeof(uint64_t));
    uint64_t * operand_extra_loc = calloc(4, sizeof(uint64_t));
    uint8_t * operand_types = calloc(4, sizeof(uint8_t));
    uint8_t operand_count = 0;



    LOG_OPCODE_INSTRUCTION("Instruction %x at 0x%lx:\n", instruction, index);

    uint64_t argument_type_index = cpu->registers.instruction_pointer;
    cpu->registers.instruction_pointer++;

    for (int i = 0; i < 2; ++i)
    {
        if (opcode_args[instruction][i*2] == 0) {
            operand_count = i * 2;
            break;
        }

        // lower nibble

        char expected_argument_type = opcode_args[instruction][i*2];

        char asbyte = memory_get_char(&cpu->memory, argument_type_index);

        operand_values[i*2] = cpu_parse_argument(cpu, asbyte & 0xF, expected_argument_type, &operand_extra_loc[i*2]);
        if (expected_argument_type == 1) 
        {
            operand_types[i*2] = asbyte & 0xF;   
        } 
        else 
        {
            operand_types[i*2] = 0xFF; // Mark as read-only
        }

        // higher nibble

        if (opcode_args[instruction][i*2+1] == 0) {
            operand_count = i * 2 + 1;
            break;
        }

        expected_argument_type = opcode_args[instruction][i*2+1];
        operand_values[i*2+1] = cpu_parse_argument(cpu, (asbyte >> 4) & 0xF, expected_argument_type, &operand_extra_loc[i*2]);
        

        if (expected_argument_type == 1) 
        {
            operand_types[i*2+1] = (asbyte >> 4) & 0xF;   
        } 
        else 
        {
            operand_types[i*2+1] = 0xFF; // Mark as read-only
        }
        

    }


    switch(instruction) {
        case OPCODE_EXIT_TO_SYSTEM:
            SET_FLAG(cpu,FLAG_EXIT);
            break;
        case OPCODE_MOV:
            operand_values[1] = operand_values[0];
            break;
        case OPCODE_DEBUG_DUMP_OPERATOR_0:
            printf("DEBUG: 0x%lx = %ld\n", operand_values[0], operand_values[0]);
            break;
        case OPCODE_JUMP:
            cpu->registers.instruction_pointer = operand_values[0];
            break;
        case OPCODE_JUMP_CONDITION:;
            char shift_value = operand_values[1] & 0x7F;
            bool flag_on = cpu->registers.flags >> shift_value & 1;
            bool invert = operand_values[1] & 0x80;
            if (flag_on ^ invert) {
                cpu->registers.instruction_pointer = operand_values[0];;
            }
            break;
        case OPCODE_BINARY_OPERATION:;
            operand_values[1] = cpu_do_operation(cpu, operand_values[0], operand_values[1], operand_values[2]);
            break;

        case OPCODE_BINARY_OPERATION_NOCHANGE:;
            cpu_do_operation(cpu, operand_values[0], operand_values[1], operand_values[2]);
            break;
        case OPCODE_IN:
            operand_values[1] = ioport_in(cpu, operand_values[0]);
            break;
        case OPCODE_OUT:
            ioport_out(cpu, operand_values[1], operand_values[0]);
            break;
        case OPCODE_PUSH:
            cpu_stack_push(cpu, operand_values[0]);
            break;
        case OPCODE_POP:
            operand_values[0] = cpu_stack_pop(cpu);
            break;
        case OPCODE_CALL:
            cpu_stack_push(cpu, cpu->registers.instruction_pointer);
            break;
        case OPCODE_RET:
            cpu->registers.instruction_pointer = cpu_stack_pop(cpu);
            break;
        case OPCODE_SYSTEM_CALL:
            operand_values[1] = la_system_call(cpu, operand_values[0], operand_values[1]);
            break;
        case OPCODE_MODE_64BIT:
            cpu->bits_mode = 64;
            break;
        case OPCODE_MODE_32BIT:
            cpu->bits_mode = 32;
            break;

        default:
            printf("%s\n", "Invalid instruction!");

    }

    // Now, update the operand values
    for(uint8_t i = 0; i < operand_count; i++) 
    {
        if (operand_types[i] == 0xFF) {
            // Don't set
            continue;
        }
        cpu_set_operand(cpu, operand_types[i], operand_extra_loc[i], operand_values[i]);
    }

    free(operand_types);
    free(operand_values);
    free(operand_extra_loc);

}
