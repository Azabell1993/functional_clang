# Makefile for building the project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -std=c11 -fexec-charset=CP949

# Executable name
TARGET = asm_main

# Source files
SRCS = asm_main.c

# Object files
OBJS = $(SRCS:.c=.o)

# DTCASE values
DTCASES = 1 2 3 4 5 6 7

# Default target
all: $(DTCASES:%=$(TARGET)%)

# Rule to build each DTCASE target
$(TARGET)%: $(SRCS)
	$(CC) $(CFLAGS) -DDTCASE=$* -o $@ $<

# Clean up
clean:
	rm -f $(TARGET)1 $(TARGET)2 $(TARGET)3 $(TARGET)4 $(TARGET)5 $(TARGET)6 $(TARGET)7 $(OBJS)
