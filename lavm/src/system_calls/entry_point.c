#include <stdio.h>
#include "cpu.h"
#include <ltdl.h>
#include "memory.h"
#include "system_calls/external_library.h"
#include "system_calls/system_call_list.h"

uint64_t la_system_call(struct cpu * cpu, uint8_t system_call_number, uint64_t arg)
{
    switch(system_call_number) 
    {
        case SYSCALL_CALL_OBJECT:;
            // Read the string in the argument
            int length = 0;

            // First, scan the string to get the length

            uint64_t index = arg;
            for (; memory_get_char(&cpu->memory, index) != 0; index++)
            {

            }

            length = index - arg;

            char * s = malloc(length);

            // Now, do the same but copy the string 

            char c;
            index = arg;
            for (; (c = memory_get_char(&cpu->memory, index)) != 0; index++)
            {
                s[index-arg] = c;
            }  



            struct module_and_symbol ret = split_library_path(s);

            printf("Object call!\n");
            printf(" Module: %s\n", ret.module);
            printf(" Symbol: %s\n", ret.symbol);
            lt_dlhandle handle  = malloc(sizeof(lt_dlhandle));
            
            if (hashmap_get(cpu->libraries, ret.module, (void**)&handle) == MAP_OK) {
                handle = malloc(sizeof(lt_dlhandle));
                hashmap_get(cpu->libraries, ret.module, (void**)&handle);
            } else {
                handle = lt_dlopen(ret.module);
                const char * err = lt_dlerror();
                if (err) {
                    printf("Error: %s\n", err );
                    exit(-1);

                }
                hashmap_put(cpu->libraries, ret.module, handle);

            }

            void (*func)(struct cpu *);
            func = lt_dlsym(handle, ret.symbol);
            func(cpu);

            break;
    }
    return 0x70D070D0;
}