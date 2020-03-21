CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic
OBJS=main.o calc_time.o

simpledu: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o simpledu

main.o: main.c global.h calc_time.h
	$(CC) $(CFLAGS) -c main.c 

calc_time.o: calc_time.h global.h calc_time.c 
	$(CC) $(CFLAGS) -c calc_time.c

clean:
	rm -f *.o *.d *.s *.i
