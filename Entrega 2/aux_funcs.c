#include "aux_funcs.h"
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void printLog(int i, int dur, int pl, char* oper)
{
    int pid = getpid();
    long int tid = pthread_self();

    printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; %s\n",time(NULL),i,pid,tid,dur,pl,oper);
}

void cleanPrintLog(void * arg)
{
    struct logInfo info = *(struct logInfo *) arg;
    printLog(info.i,info.dur,info.pl,"FAILD");
}

void cleanUnlockMutex(void * arg)
{
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

void cleanUnlinkFifo(void * arg)
{
    unlink((char *)arg);
}

void cleanCloseFile(void * arg)
{
    fclose((FILE *)arg);
}

void sigusr1_handler(int signo)
{
    if (signo != SIGUSR1) fprintf(stderr,"This handler shouldn't have been called.\n");
}

void sigusr2_handler(int signo)
{
    if (signo != SIGUSR2) fprintf(stderr,"This handler shouldn't have been called.\n"); 
    pthread_exit(0);
}