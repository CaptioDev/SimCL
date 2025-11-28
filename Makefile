CC = cc
CFLAGS = -Wall -Wextra -std=c89 -pedantic
INCLUDES = -Iinclude

SRC = \
    src/main.c \
    src/lexer.c \
    src/parser.c \
    src/ast.c \
    src/semantic.c \
    src/type_system.c \
    src/symbol_table.c \
    src/ir.c \
    src/optimizer.c \
    src/codegen.c \
    src/bytecode.c \
    src/vm.c \
    src/runtime.c \
    src/linalg.c \
    src/solvers.c \
    src/random.c \
    src/allocator.c \
    src/profiling.c \
    src/threading.c \
    src/std_io.c \
    src/std_math.c \
    src/std_array.c

OBJ = $(SRC:.c=.o)

all: simcl

simcl: $(OBJ)
	$(CC) $(CFLAGS) -o simcl $(OBJ)

clean:
	rm -f $(OBJ) simcl
