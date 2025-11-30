# Makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = myshell
OBJS = myshell.o cmd_file.o cmd_info.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

cmd_file.o: cmd_file.c myshell.h
	$(CC) $(CFLAGS) -c cmd_file.c

cmd_info.o: cmd_info.c myshell.h
	$(CC) $(CFLAGS) -c cmd_info.c

clean:
	rm -f $(OBJS) $(TARGET)