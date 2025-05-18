CC = gcc
CFLAGS = -Wall

all: treasure_manager treasure_hub score_calculator

treasure_manager: treasure_manager.o treasure.o
	$(CC) $(CFLAGS) -o treasure_manager treasure_manager.o treasure.o

treasure_hub: treasure_hub.o
	$(CC) $(CFLAGS) -o treasure_hub treasure_hub.o

treasure_manager.o: treasure_manager.c treasure.h
	$(CC) $(CFLAGS) -c treasure_manager.c

treasure.o: treasure.c treasure.h
	$(CC) $(CFLAGS) -c treasure.c

treasure_hub.o: treasure_hub.c
	$(CC) $(CFLAGS) -c treasure_hub.c -o treasure_hub.o

score_calculator: score_calculator.o
	$(CC) $(CFLAGS) -o score_calculator score_calculator.o

score_calculator.o: score_calculator.c
	$(CC) $(CFLAGS) -c score_calculator.c -o score_calculator.o

clean:
	rm -f *.o treasure_manager treasure_hub score_calculator