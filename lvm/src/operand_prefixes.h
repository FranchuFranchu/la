#define OPERAND_REGISTER_A 1
#define OPERAND_REGISTER_B 2
#define OPERAND_REGISTER_C 3
#define OPERAND_REGISTER_D 4
#define OPERAND_REGISTER_E 5 
#define OPERAND_REGISTER_F 6
#define OPERAND_REGISTER_G 7
#define OPERAND_REGISTER_H 8
#define OPERAND_REGISTER_I 9
#define OPERAND_REGISTER_FLAGS 0xA
#define OPERAND_HIGHEST_REGISTER OPERAND_REGISTER_FLAGS
// These take a doubleword 
#define OPERAND_SPECIAL_MEMORY_RELATIVE 0xb
#define OPERAND_SPECIAL_MEMORY_ABSOLUTE 0xc
#define OPERAND_SPECIAL_IMMEDIATE 0xd
#define OPERAND_HIGHEST_DWORDARG OPERAND_SPECIAL_IMMEDIATE
// These take a doubleword and a half-byte
#define OPERAND_SPECIAL_MEMORY_OFFSET_FROM_REGISTER 0xe
#define OPERAND_SPECIAL_MEMORY_FROM_REGISTER 0xf
#define OPERAND_HIGHEST_DWORD_CHAR OPERAND_SPECIAL_MEMORY_FROM_REGISTER