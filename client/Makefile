TARGET=client
OBJS=main.o $(TARGET).o queue.o message.o led_matrix.o joystick.o 
OBJS+=game_basis.o slide_master.o high_or_low.o rainfall.o

all:
	gcc -c ../src/$(TARGET).c -o $(TARGET).o -I../game/ -I../hat/

	gcc -c ../src/queue.c -o queue.o
	gcc -c ../src/message.c -o message.o
	gcc -c ../hat/led_matrix.c -o led_matrix.o
	gcc -c ../hat/joystick.c -o joystick.o

	gcc -c ../game/game_basis.c -o game_basis.o -I../hat/
	gcc -c ../game/slide_master.c -o slide_master.o -I../hat/
	gcc -c ../game/high_or_low.c -o high_or_low.o -I../hat/
	gcc -c ../game/rainfall.c -o rainfall.o -I../hat/

	gcc -c main.c -o main.o -I../src/ -I../game/ -I../hat/
	
	gcc -o $(TARGET) $(OBJS) -lpthread

clean:
	rm -f $(TARGET) $(OBJS)
