# Makefile

# Setting the compiler and the default linker program
CC = gcc
# options for pre-processor (-I, -include, -D ... )
CPPFLAGS =
# main compilation options
CFLAGS = -Wall -Wextra -Werror -Wvla -std=c99 -pedantic #-g -fsanitize=address
# Linker options 
LDFLAGS = #-fsanitize=address -lcriterion
# libs and path for linker
LDLIBS =

BIN = myfind

OBJ = src/main.o \
	  src/error.o \
	  src/options.o \
	  src/action.o \
	  src/compute.o \
	  src/tokens.o \
	  src/lexer.o \
	  src/expressions.o \
	  src/ast.o \
	  src/utils/file_management.o \
	  src/utils/namelist.o \
	  src/utils/stack.o

all: myfind

myfind: ${OBJ}
	${CC} ${LDFLAGS} ${OBJ} -o ${BIN}

check: myfind
	tests/./tests.sh

.PHONY: all clean

clean:
	${RM} ${OBJ}   # remove object files
	${RM} ${BIN}   # remove main program
	${RM} tests/.*.out
	${RM} .*.out

# END
