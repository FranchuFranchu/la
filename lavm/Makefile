CC_FLAGS=-Wall -Wextra -Isrc/
CC_LIBRARIES=-lltdl -ldl
CC=gcc

BINFMT_ENTRY=":lavm:M:0:\\x7FELF\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00LA:\\xFF\\xFF\\xFF\\xFF\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\xFF\\xFF:"$(shell pwd)"/"$(PROG_NAME)":P"

PROG_ARGS=../la_generated_bytecode.lac
PROG_NAME=lavm

OBJECT_DIR=object

_OBJ =  cpu main memory cpu_operation ioport cpu_stack \
		system_calls/external_library system_calls/entry_point \
		util_datatypes/hashmap \
		elf_file_parsing

OBJ = $(patsubst %,$(OBJECT_DIR)/%.o,$(_OBJ))


$(OBJECT_DIR)/%.o: src/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $< $(CFLAGS)

link: $(OBJ)
	$(CC) -o $(PROG_NAME) $(OBJ) $(CC_LIBRARIES)

all: $(OBJ) link

run: all
	./$(PROG_NAME) $(PROG_ARGS)

clean:
	rm -r object/*
	mkdir -p $(OBJECT_DIR)
	mkdir -p $(OBJECT_DIR)/system_calls
	mkdir -p $(OBJECT_DIR)/util_datatypes

register_binary_format:
	sudo bash -c "echo \"$(BINFMT_ENTRY)\" > /proc/sys/fs/binfmt_misc/register"
