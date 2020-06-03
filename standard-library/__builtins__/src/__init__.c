#include <stdio.h>
#include "cpu.h"


void la_debug(struct cpu * cpu) 
{
    printf("%s: %ld\n", "DEBUG AAA", cpu->registers.a);
}