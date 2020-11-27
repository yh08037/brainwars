TARGET=client

all:
	gcc -c ../src/$(TARGET).c -o $(TARGET).o
	gcc -c ../src/queue.c -o queue.o
	gcc -c ../src/message.c -o message.o
	gcc -c ../game/led_matrix.c -o led_matrix.o
	gcc -c ../game/joystick.c -o joystick.o
	gcc -c ../game/slide_master.c -o slide_master.o
	gcc -c main.c -o main.o -I../src/
	gcc -o $(TARGET) main.o $(TARGET).o queue.o message.o led_matrix.o joystick.o slide_master.o -lpthread

clean:
	rm -f $(TARGET) main.o $(TARGET).o queue.o message.o slide_master.o
