#define LOG_ERROR(fmt, ...) if (cpu->debug_settings.log_errors) { printf(fmt, ##__VA_ARGS__);}
#define LOG_OPCODE_ARGUMENT(fmt, ...) if (cpu->debug_settings.log_opcode_arguments) { printf(fmt, ##__VA_ARGS__);}
#define LOG_OPCODE_INSTRUCTION(fmt, ...) if (cpu->debug_settings.log_opcode_instructions) { printf(fmt, ##__VA_ARGS__);}
#define LOG_MEMORY_READ(fmt, ...) if (memory->debug_settings.log_reads) { printf(fmt, ##__VA_ARGS__);}
#define LOG_MEMORY_WRITE(fmt, ...) if (memory->debug_settings.log_writes) { printf(fmt, ##__VA_ARGS__);}