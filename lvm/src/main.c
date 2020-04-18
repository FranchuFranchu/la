#include <stdio.h>
#include "cpu.h"

int main(int argc, char const *argv[])
{
    if (argc < 2){
        printf("%s\n", "Not enough arguments!");
        return 1;
    }
    struct cpu * cpu = cpu_init(argv[1]);
    cpu_run(cpu);
    return 0;
}
