CC=gcc

# 

CFLAGS=-std=c99 -Wextra -Wall -pedantic
LDFLAGS=

# 

EXEC=stack_ex1

# 

INCLUDE_DIRECTORY=./
INCLUDES=$(shell find $(INCLUDE_DIRECTORY) -type f -name '*.h')

# 

MODULE_DIRECTORY=./
MODULES=$(shell find $(MODULE_DIRECTORY) -type f -name '*.c')

# 

OBJECT_DIRECTORY=./
OBJECTS=$(subst .c,.o,$(subst modules/,objects/,$(MODULES)))
MODULE_OBJECTS=$(MODULES:.c=.o)

# 

ifeq ($(DEBUG),yes)
	CFLAGS += -g
	LDFLAGS =
else
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS =
endif

# 

all: $(EXEC) compile

# 

compile:
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXEC) $(LDFLAGS)

# 

$(EXEC): $(OBJECTS)

main.o: main.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJECT_DIRECTORY)%.o: $(MODULE_DIRECTORY)%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# 

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

# 

doc: main.c stack.h
	doxygen documentation/TP1

serve-doc: doc
	http-server -p 8080 documentation/html -c-1