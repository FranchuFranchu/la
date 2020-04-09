#include <stdio.h>
#include "cpu.h"

int main(int argc, char const *argv[])
{
    struct cpu * cpu = cpu_init("la_bytecode.lac");
    cpu_run(cpu);
    return 0;
}
