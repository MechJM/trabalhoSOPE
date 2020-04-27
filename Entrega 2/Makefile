CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -pthread -lrt
OBJS_U=Un.o 
OBJS_Q1=Q1.o

Q1: $(OBJS_Q1)
	$(CC) $(CFLAGS) $(OBJS_Q1) -o Q1

Q1.o: Q1.c
	$(CC) $(CFLAGS) -c Q1.c

U1: $(OBJS_U)
	$(CC) $(CFLAGS) $(OBJS_U) -o U1

U2: $(OBJS_U)
	$(CC) $(CFLAGS) $(OBJS_U) -o U2

Un.o: Un.c   
	$(CC) $(CFLAGS) -c Un.c 

clean:
	rm -f *.o *.d *.s *.i
