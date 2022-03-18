CC = gcc
CFLAGS = -Wall -g -pthread
SRC=src
OBJ=obj
BIN=bin
LIB=lib
LIBSRC=src/libthreadpool

# Contains all the source files in src/ directory
SRCFILES = $(wildcard src/*.c)
LIBSRCFILES = $(wildcard $(LIBSRC)/*.c)
# patsubst takes 3 args: pattern, replacement and text
# It searches for src/%.c pattern (basically all the c source files in src directory)
# and replaces it with bin/% where % is replaced by filename in pattern and input text is all the SRCFILES 
EXECS = $(patsubst src/%.c,bin/%,$(SRCFILES))
SHAREDOBJECTS = $(LIB)/libthreadpool.so
# The next line just defines all the Object files from exec pattern
OBJS = $(patsubst %,%.o,$(EXECS))

.PHONY: all
all: $(SHAREDOBJECTS) $(EXECS)

$(EXECS): $(BIN)/%: $(OBJ)/%.o
	$(CC) $(CFLAGS) -L ./lib -o $@ $< -lthreadpool

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -L ./lib -c $< -o $@ -lthreadpool

$(SHAREDOBJECTS): $(LIBSRCFILES)
	$(CC) $(CCFLAGS) -shared -fPIC -o $(LIB)/libthreadpool.so $(LIBSRCFILES)

.PHONY: clean
clean:
	rm -rf obj/*
	rm -rf bin/*
	rm -rf lib/*
