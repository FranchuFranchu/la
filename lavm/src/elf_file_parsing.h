#include <stdio.h>

#include "memory.h"

void elf_load_file(struct memory * memory, FILE * file);

struct elf_file_header {
    uint32_t magic_number;
    uint8_t bits;
    uint8_t endianess;
    uint8_t elf_version_char;
    uint8_t os_abi;
    uint8_t os_abi_version;
    uint8_t padding[7];
    uint16_t file_type;
    uint16_t instruction_set;
    uint16_t elf_version_word;

    uint64_t entry_point;
    uint64_t program_header_position;
    uint64_t section_header_position;
    uint32_t flags;

    uint16_t header_size;
    uint16_t program_entry_size;
    uint16_t program_entry_count;
    uint16_t section_entry_size;
    uint16_t section_entry_count;
    uint16_t section_name_section;

};

struct elf_section_header {
    uint32_t name_offset;
    uint32_t type;
    uint64_t flags;
    uint64_t virtual_address;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t align;
    uint64_t entry_size;

};