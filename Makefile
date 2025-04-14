# Compiler
CC = gcc

# Compiler options:
CFLAGS = -Wall -g -c

# Linker options:
LDFLAGS = -lm

# Executable file name
PROGRAM = main

# Pthread library option
PTHREAD = -lpthread

# Links for real-time library
LINKS = -lrt

# Object files
OBJS = main.o file.o parent.o child.o process.o semaphores.o shared_memory.o 

# Build the program
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(PROGRAM) $(PTHREAD) $(LINKS)

# Build object files
program.o: program.c
	$(CC) $(CFLAGS) program.c -o program.o

# Clean target
clean:
	rm -f $(PROGRAM) $(OBJS)