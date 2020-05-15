#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <ltdl.h> // For loading C libraries at runtime
// https://www.gnu.org/software/libtool/manual/html_node/Libltdl-interface.html

#include "cpu.h"

#define LA_PATH "../standard-library/"

void load_library(struct cpu * cpu, char * module_name)
{   
    size_t module_name_length = strlen(module_name);
    size_t length = strlen(LA_PATH) + 2;
    // Try to find the directory corresponding to the module
    char * la_path = malloc(length);
    strcpy(la_path, LA_PATH);

    // Add current working directory too
    la_path[length-3] = ':';
    la_path[length-2] = '.';
    la_path[length-1] = 0;

    printf("%d\n", 1);

    int start_index = 0;
    char c;
    char * concatenated_string;
    char * full_module_path = NULL;
    for (int i = 0; (c = la_path[i]) != 0; ++i)
    {
        if (c == ':') { // Path separator


            la_path[i] = 0;

            printf("%d\n", 2);
            size_t this_path_length = i - start_index;
            concatenated_string = malloc(this_path_length + 1 + module_name_length + 1);
            printf("%d\n", 3);
            strcpy(concatenated_string, la_path + start_index);
            concatenated_string[this_path_length] = '/';
            strcpy(concatenated_string + this_path_length + 1, module_name);
            printf("%s\n", concatenated_string);

            la_path[i] = '/';


            if( access(concatenated_string, F_OK ) != -1 ) 
            {
                // file exists
                full_module_path = concatenated_string;
                break;
            } 
            else 
            {
                // file doesn't exist
                free(concatenated_string);
                start_index = i;
                continue;
            }
        }
    }
    if (full_module_path == NULL) {
        printf("Error: Module doesn't exist!\n");
    }
    printf("%s\n", "Reached end!");


    free(full_module_path);
}