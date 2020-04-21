#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS 100000
#define STR_LEN 100

char fifoname[STR_LEN] = "";
int flag = 1;

void sigalrm_handler(int signo)
{
    if (signo != SIGALRM)
    {
        fprintf(stderr,"This handler shouldn't have been called.\n");
        return;
    }

    flag = 0;
}

void* threadFunc(void * arg)
{
    arg = arg; //without this the compiler reports an error about an unused parameter
    int duration = rand() % 20 + 1;


    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"Usage: U1 <-t nsecs> fifoname\n");
        exit(1);
    }

    srand(time(NULL));

    int nsecs;

    for (int i = 1; argv[i] != NULL; i++)
    {
        if (strcmp("-t",argv[i]) == 0)
        {
            i++;
            nsecs = atoi(argv[i]);
        }
        else strcpy(fifoname,argv[i]);
    }

    struct sigaction action;
    action.sa_handler = sigalrm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGALRM,&action,NULL) < 0)
    {
        fprintf(stderr,"Couldn't install signal handler.\n");
        exit(1);
    }

    alarm(nsecs);

    pthread_t tids[NUM_THREADS];

    int i = 0;
    while (flag)
    {
        if (pthread_create(&tids[i],NULL,threadFunc,NULL) != 0)
        {
            fprintf(stderr,"Couldn't create thread.\n");
            exit(1);
        }
        i++;
        usleep(5000);
    }

    /*
    for (int i2 = 0; i2 < i; i2++)
    {
        pthread_join(tids[i2],NULL);
    }
    */

    return 0;
    //pthread_exit(0);
}