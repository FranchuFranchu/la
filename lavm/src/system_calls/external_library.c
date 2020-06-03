#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ltdl.h> // For loading C libraries at runtime
// https://www.gnu.org/software/libtool/manual/html_node/Libltdl-interface.html

#include "cpu.h"
#include "system_calls/external_library.h"

#define LA_PATH "../standard-library/"

struct module_and_symbol split_library_path(char * module_name)
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


    int slash_seeking_i;

    int start_index = 0;
    char c;
    char * concatenated_string;
    char * full_module_path = NULL;

    for (int i = 0; (c = la_path[i]) != 0; ++i)
    {
        if (c == ':') { // Path separator


            la_path[i] = 0;

            size_t this_path_length = i - start_index;
            concatenated_string = malloc(this_path_length + 1 + module_name_length + 1);

            strcpy(concatenated_string, la_path + start_index);
            concatenated_string[this_path_length] = '/';
            strcpy(concatenated_string + this_path_length + 1, module_name);
            concatenated_string[this_path_length + strlen(module_name)] = 0;


            la_path[i] = '/';

            // Now that we have a concatenated string
            // let's replace the slashes by null terminators
            // and find the first match

            char slash_seeking_char;

            struct stat path_stat;
            for (slash_seeking_i = 0; (slash_seeking_char = concatenated_string[slash_seeking_i]) != 0; ++slash_seeking_i)
            {
                if (slash_seeking_char == '/')  {
                    // Replace by a 0
                    concatenated_string[slash_seeking_i] = 0;

                    stat(concatenated_string, &path_stat);
                    if( S_ISREG(path_stat.st_mode)) 
                    {
                        // file exists
                        full_module_path = concatenated_string;
                        
                        goto break_all_loops;
                    } 
                    else 
                    {
                        // file doesn't exist
                        // Continue
                        concatenated_string[slash_seeking_i] = '/';
                    }


                }
            }

        }
    }
    break_all_loops:

    if (full_module_path == NULL) {
        printf("Error: Module doesn't exist!\n");
        exit(-1);
    }

    struct module_and_symbol ret = {
        .module = concatenated_string, 
        .symbol = concatenated_string + slash_seeking_i + 1
    };
    return ret;
}