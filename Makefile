# Makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = myshell
OBJS = main.o execute.o commands.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c myshell.h
	$(CC) $(CFLAGS) -c main.c

execute.o: execute.c myshell.h
	$(CC) $(CFLAGS) -c execute.c

commands.o: commands.c myshell.h
	$(CC) $(CFLAGS) -c commands.c

clean:
	rm -f *.o $(TARGET)