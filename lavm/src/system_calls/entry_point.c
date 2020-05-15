#include <stdio.h>
#include "cpu.h"

uint32_t la_system_call(struct cpu * cpu, uint8_t system_call_number, uint32_t arg)
{
    printf("%d\n", system_call_number);
    return 0x70D070D0;
}