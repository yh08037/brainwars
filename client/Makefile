TARGET=client
OBJS=main.o $(TARGET).o queue.o message.o led_matrix.o joystick.o game_basis.o slide_master.o high_or_low.o

all:
	gcc -c ../src/$(TARGET).c -o $(TARGET).o -I../game/
	gcc -c ../src/queue.c -o queue.o
	gcc -c ../src/message.c -o message.o
	gcc -c ../game/led_matrix.c -o led_matrix.o
	gcc -c ../game/joystick.c -o joystick.o
	gcc -c ../game/game_basis.c -o game_basis.o
	gcc -c ../game/slide_master.c -o slide_master.o
	gcc -c ../game/high_or_low.c -o high_or_low.o
	gcc -c main.c -o main.o -I../src/ -I../game/
	gcc -o $(TARGET) $(OBJS) -lpthread

clean:
	rm -f $(TARGET) $(OBJS)
