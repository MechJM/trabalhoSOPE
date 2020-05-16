CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -pthread -lrt
OBJS_U=Un.o
OBJS_U2=Un.o aux_funcs.o
OBJS_Q1=Q1.o
OBJS_Q2=Q2.o queue.o

default: Q2 U2

Q1: $(OBJS_Q1)
	$(CC) $(CFLAGS) $(OBJS_Q1) -o Q1

Q2: $(OBJS_Q2)
	$(CC) $(CFLAGS) $(OBJS_Q2) -o Q2

Q1.o: Q1.c
	$(CC) $(CFLAGS) -c Q1.c

Q2.o: Q2.c queue.h
	$(CC) $(CFLAGS) -c Q2.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

U1: $(OBJS_U)
	$(CC) $(CFLAGS) $(OBJS_U) -o U1

U2: $(OBJS_U2)
	$(CC) $(CFLAGS) $(OBJS_U2) -o U2

Un.o: Un.c aux_funcs.h
	$(CC) $(CFLAGS) -c Un.c 

aux_funcs.o: aux_funcs.c aux_funcs.h
	$(CC) $(CFLAGS) -c aux_funcs.c

clean:
	rm -f *.o *.d *.s *.i
