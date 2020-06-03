from typing import List
from dataclasses import dataclass, field
import struct

ELF_SIGNATURE = b'\x7FELF'
LA_SIGNATURE = b'LA'

def pack_section_header(d):

    section_header = struct.pack('@IILLLLIILL', 
        d["name_offset"],
        d["type"],
        d["flags"],
        d["virtual_address"],
        d["offset"],
        d["size"],
        d["link"],
        d["info"],
        d["align"],
        d["entry_size"],
    )
    return section_header

def pack_program_header(d):

    program_header = struct.pack('@ILLLLLIL', 
        d["type"],
        d["offset"],
        d["virtual_address"],
        d["physical_address"],
        d["file_size"],
        d["virtual_size"],
        d["flags"],
        d["align"],
    )
    return program_header

def pack_elf_header(d):


    header = struct.pack('@I BBBBB xxxxxxx HHI LLLI HHHHHH ', 
        d["magic_number"],

        d["bits"],
        d["endianess"],
        d["elf_version"],
        d["os_abi"],
        d["os_abi_version"],

        d["file_type"],
        d["instruction_set"],
        d["elf_version"],

        d["entry_point"],
        d["program_header_position"],
        d["section_header_position"],
        d["flags"],

        d["header_size"],
        d["program_entry_size"],
        d["program_entry_count"],
        d["section_entry_size"],
        d["section_entry_count"],
        d["section_name_section"],
    )
    return header

from dataclasses import dataclass

@dataclass
class ElfSection:
    name: str 
    content: bytes
    type: int = 1
    flags: int = 1 | 2 | 4 
    virtual_address: int = 0
    link: int = 0
    info: int = 0
    align: int = 1
    entry_size: int = 0

    def pack_header(self, offset, name_offset):
        return pack_section_header({
            **self.__dict__, 
            "name_offset": name_offset,
            "offset": offset, 
            "size": len(self.content)
        })
    
@dataclass
class ElfFile:
    sections: List[ElfSection] = field(default_factory = list)
    def write_to(self, file):
        header_dictionary = {
            "magic_number": int.from_bytes(ELF_SIGNATURE, byteorder='little'),
            "endianess": 1,
            "os_abi": int.from_bytes(b'L', byteorder='little'),
            "os_abi_version": 0,
            "instruction_set": int.from_bytes(LA_SIGNATURE, byteorder='little'),
            "bits": 2,
            "file_type": 2,
            "elf_version": 1,
            "entry_point": 0xdeadc0de,
            "program_header_position": 0,
            "section_header_position": 0x40,
            "flags": 0x0000,
            "header_size": 0x40,
            "section_name_section": 1,
            "program_entry_size": 0x40,
            "program_entry_count": 0,
            "section_entry_size": 0x40,
            "section_entry_count": len(self.sections),
        }

        used_header_space = 0x40
        section_headers = bytes()


        section_name_section_name = '.shstrtab'

        names = b'\0' + section_name_section_name.encode('utf-8') + b'\0'
        name_locations = {
            section_name_section_name: 1,
            '':0,
        }


        for i in self.sections:
            name_locations[i.name] = len(names)
            names += i.name.encode('utf-8') + b'\0'

        self.sections.insert(0, ElfSection(name=section_name_section_name, content='', flags=0, type=3))
        header_dictionary["section_entry_count"] += 2
        self.sections[0].content = names
        self.sections.insert(0, ElfSection(name='', content=b'', flags=0, type=0))

        data_start = (0
            + header_dictionary["section_entry_count"] * header_dictionary["section_entry_size"] 
            + header_dictionary["program_entry_count"] * header_dictionary["program_entry_size"] 
            + header_dictionary["header_size"]
        )

        data_area = b''

        for i in self.sections:
            header = i.pack_header(data_start, name_offset=name_locations[i.name])
            data_area += i.content
            data_start += len(i.content)
            section_headers += header


        file.write(pack_elf_header(header_dictionary) + section_headers + data_area)




