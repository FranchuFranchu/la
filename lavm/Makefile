CC_FLAGS=-Wall -Wextra -Isrc/
CC_LIBRARIES=-lltdl
CC=gcc

PROG_ARGS=../la_generated_bytecode.lac
PROG_NAME=lavm

OBJECT_DIR=object

_OBJ = cpu main memory cpu_operation ioport cpu_stack system_calls/external_library system_calls/entry_point
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



