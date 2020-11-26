TARGET=server

all:
	gcc -c ../src/$(TARGET).c -o $(TARGET).o
	gcc -c ../src/queue.c -o queue.o
	gcc -c main.c -o main.o -I../src/
	gcc -o $(TARGET) main.o $(TARGET).o queue.o -lpthread

clean:
	rm -f $(TARGET) main.o $(TARGET).o queue.o
