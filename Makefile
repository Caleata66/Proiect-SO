CC = gcc
CFLAGS = -Wall

all: treasure_manager treasure_hub

treasure_manager: treasure_manager.o treasure.o
	$(CC) $(CFLAGS) -o treasure_manager treasure_manager.o treasure.o

treasure_hub: treasure_hub.o
	$(CC) $(CFLAGS) -o treasure_hub treasure_hub.o

treasure_manager.o: treasure_manager.c treasure.h
	$(CC) $(CFLAGS) -c treasure_manager.c

treasure.o: treasure.c treasure.h
	$(CC) $(CFLAGS) -c treasure.c

treasure_hub.o: treasure_hub.c
	$(CC) $(CFLAGS) -c treasure_hub.c

clean:
	rm -f *.o treasure_manager treasure_hub