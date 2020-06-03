#include "cpu.h"

struct module_and_symbol split_library_path(char * name);

struct module_and_symbol {
    char * module;
    char * symbol;
};