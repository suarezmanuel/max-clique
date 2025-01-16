CC      = gcc
CFLAGS  = -O0        # compiler flags (for compilation)
LDFLAGS = -lm
OBJ     = set.o graph.o
EXEC    = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJ)
	rm -f stupid.o set.o graph.o 

graph.o: graph.c
	$(CC) $(CFLAGS) -c graph.c -o graph.o

set.o: set.c
	$(CC) $(CFLAGS) -c set.c -o set.o

stupid.o: stupid.c
	$(CC) $(CFLAGS) -c stupid.c -o stupid.o

run:
	./main

clean:
	rm -f $(EXEC) $(OBJ)