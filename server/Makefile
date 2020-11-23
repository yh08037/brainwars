
CC=gcc
CFLAGS=-c
LFLAGS=-lpthread
TARGET=server
OBJS=main.o server.o queue.o

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
