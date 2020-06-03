#include <stdio.h>

#include "memory.h"
#include "elf_file_parsing.h"

#define ELF_MAGIC 0x464c457f
#define LA_INSTRUCTION_SET 0x414c
#define ELF_SECTION_FLAG_ALLOC 2

void elf_load_file(struct memory * memory, FILE * file) {
    // Copy file contents to memory to memory
    struct elf_file_header * header = malloc(sizeof(struct elf_file_header));

    fread(header, sizeof(struct elf_file_header), 1, file);

    if (header->magic_number != ELF_MAGIC) {
        printf("Error loading program, magic number 0x%x is not an ELF signature\n", header->magic_number);
        exit(-1);
    }

    if (header->instruction_set != LA_INSTRUCTION_SET) {
        printf("Error loading program, instruction set 0x%x is not the La instruction set\n", header->instruction_set);
        exit(-1);
    }

    // Iterate through all sections

    fseek(file, header->section_header_position, SEEK_SET);

    struct elf_section_header * section_header = malloc(sizeof(struct elf_section_header));

    int index;

    for (int i = 0; i < header->section_entry_count; i++)
    {
        index = header->section_header_position + i * header->section_entry_size;

        fseek(file, index, SEEK_SET);
        fread(section_header, sizeof(struct elf_section_header), 1, file);
        printf("%d\n", section_header->type);

        if (section_header->flags & ELF_SECTION_FLAG_ALLOC) 
        {
            fseek(file, section_header->offset, SEEK_SET);

            int memory_index = section_header->virtual_address;

            for (uint64_t j = 0; j < section_header->size; j++)
            {
                int c = fgetc(file);
                if (c > 0xFF)
                {
                    printf("Error: Unexpected EOF.\n");
                    exit(-1);
                }
                printf("Loading 0x%x at 0x%x\n", c, memory_index);
                memory_set_char(memory, memory_index, c & 0xFF);
                memory_index++;
                
            }
        }
        /* code */
    }

    free(section_header);
    free(header);

    fclose(file);

}
