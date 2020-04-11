CC=gcc
CFLAGS=

OBJECT_DIR=object

_OBJ = cpu main memory cpu_operation
OBJ = $(patsubst %,$(OBJECT_DIR)/%.o,$(_OBJ))

$(OBJECT_DIR)/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

link: $(OBJ)
	gcc -o lvm $(OBJ)

all: $(OBJ) link

run: all
	./lvm



