CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -pthread -lrt
OBJS=Un.o 

U1: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o U1

U2: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o U2

Un.o: Un.c   
	$(CC) $(CFLAGS) -c Un.c 

clean:
	rm -f *.o *.d *.s *.i
