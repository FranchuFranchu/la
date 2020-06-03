#include <stdio.h>
#include <ltdl.h>
#include <dlfcn.h>


#include "cpu.h"
#include "system_calls/external_library.h"

int main(int argc, char const *argv[])
{
    lt_dlinit();    
    lt_dlsetsearchpath(".");
    if (argc < 2){
        printf("%s\n", "Not enough arguments!");
        return 1;
    }

    struct cpu * cpu = cpu_init(argv[1]);
    cpu_run(cpu);

    return 0;
}
