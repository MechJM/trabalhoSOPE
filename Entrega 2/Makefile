CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -pthread -lrt
OBJS_U=Un.o 

U1: $(OBJS_U)
	$(CC) $(CFLAGS) $(OBJS_U) -o U1

U2: $(OBJS_U)
	$(CC) $(CFLAGS) $(OBJS_U) -o U2

Un.o: Un.c   
	$(CC) $(CFLAGS) -c Un.c 

clean:
	rm -f *.o *.d *.s *.i
