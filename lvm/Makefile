CC=gcc
CFLAGS=

PROG_ARGS=../la_generated_bytecode.lac
PROG_NAME=lvm

OBJECT_DIR=object

_OBJ = cpu main memory cpu_operation ioport
OBJ = $(patsubst %,$(OBJECT_DIR)/%.o,$(_OBJ))

$(OBJECT_DIR)/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

link: $(OBJ)
	gcc -o $(PROG_NAME) $(OBJ)

all: $(OBJ) link

run: all
	./$(PROG_NAME) $(PROG_ARGS)



