
CC=gcc
CFLAGS=-c
LFLAGS=-lpthread
TARGET=client
OBJS=main.o client.o queue.o

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
