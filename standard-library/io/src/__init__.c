#include <unistd.h>
#include "cpu.h"


// IN = A: File descriptor, B & 0xFF: Byte to write
// OUT = A: Bytes written
void la_write_byte(struct cpu * cpu) 
{
    char * buffer = malloc(1);
    cpu->registers.a = write(cpu->registers.a & 0U - 1,buffer,1)
    free(buffer);
}

// IN = A: File descriptor
// OUT = B & 0xFF: Byte read, A: Bytes read
void la_read_byte(struct cpu * cpu) 
{
    char * buffer = malloc(1);
    cpu->registers.a = write(cpu->registers.a & 0U - 1,buffer,1);
    cpu->registers.b |= *buffer;
    free(buffer);
}