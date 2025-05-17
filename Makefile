CC = gcc
CFLAGS = -Wall

OBJS = treasure_manager.o treasure.o

all: treasure_manager

treasure_manager: $(OBJS)
	$(CC) $(CFLAGS) -o treasure_manager $(OBJS)

treasure_manager.o: treasure_manager.c treasure.h
	$(CC) $(CFLAGS) -c treasure_manager.c

treasure.o: treasure.c treasure.h
	$(CC) $(CFLAGS) -c treasure.c

clean:
	rm -f *.o treasure_manager