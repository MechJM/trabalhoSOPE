CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic
OBJS=main.o calc_time.o disk_usage.o log_file.o signal_handling.o

simpledu: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o simpledu

main.o: main.c global.h calc_time.h
	$(CC) $(CFLAGS) -c main.c 

calc_time.o: calc_time.h global.h calc_time.c 
	$(CC) $(CFLAGS) -c calc_time.c

disk_usage.o: disk_usage.c disk_usage.h global.h
	$(CC) $(CFLAGS) -c disk_usage.c

log_file.o: log_file.c log_file.h global.h
	$(CC) $(CFLAGS) -c log_file.c

signal_handling.o: signal_handling.c global.h signal_handling.h
	$(CC) $(CFLAGS) -c signal_handling.c

clean:
	rm -f *.o *.d *.s *.i
