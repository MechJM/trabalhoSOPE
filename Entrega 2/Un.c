#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define NUM_THREADS 100000
#define STR_LEN 100

char fifoname[STR_LEN] = "";
int seqNum = 1;
int flag = 1;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutFifo = PTHREAD_MUTEX_INITIALIZER;


void * countTime(void * arg)
{
    int time = *((int * )arg);

    struct timespec time1,time2;

    time1.tv_sec = time;
    time1.tv_nsec = 0;

    if (nanosleep(&time1,&time2) < 0)
    {
        fprintf(stderr,"Couldn't sleep.\n");
        pthread_exit(0);
    }
    flag = 0;
    return NULL;
}

void* threadFunc(void * arg)
{
    arg = arg; //without this the compiler reports an error about an unused parameter

    pthread_mutex_lock(&mut);
    int i = seqNum++;
    pthread_mutex_unlock(&mut);

    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    int dur = rand() % 20 + 1;
    int pl = -1;

    char ansFifoName[STR_LEN] = "";
    sprintf(ansFifoName,"/tmp/%d.%ld",pid,tid);
    if (mkfifo(ansFifoName,0600) < 0)
    {
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; FAILD\n",time(NULL),i,pid,tid,dur,pl);
        pthread_exit(0);
    }

    pthread_mutex_lock(&mutFifo);
    FILE* reqFifoPtr = fopen(fifoname,"w");
    if (reqFifoPtr == NULL)
    {
        pthread_mutex_unlock(&mutFifo);
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; FAILD\n",time(NULL),i,pid,tid,dur,pl);
        unlink(ansFifoName);
        pthread_exit(0);
    }

    if (fprintf(reqFifoPtr,"[ %d , %d , %ld , %d , %d ]\n",i,pid,tid,dur,pl) < 0)
    {
        pthread_mutex_unlock(&mutFifo);
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; FAILD\n",time(NULL),i,pid,tid,dur,pl);
        unlink(ansFifoName);
        pthread_exit(0);
    }
    fclose(reqFifoPtr);
    pthread_mutex_unlock(&mutFifo);
    printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; IWANT\n",time(NULL),i,pid,tid,dur,pl);

    FILE* ansFifoPtr = fopen(ansFifoName,"r");
    if (ansFifoPtr == NULL) 
    {
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; FAILD\n",time(NULL),i,pid,tid,dur,pl);
        unlink(ansFifoName);
        pthread_exit(0);
    }
    
    char answer[STR_LEN] = "";
    fgets(answer,STR_LEN,ansFifoPtr);
    fclose(ansFifoPtr);
    
    if (strstr(answer,"-1") != NULL) 
    {
        pthread_mutex_lock(&mut);
        flag = 0;
        pthread_mutex_unlock(&mut);
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; CLOSD\n",time(NULL),i,pid,tid,dur,pl);
    }
    else
    {
        int buffer,buffer2,buffer4;
        long int buffer3;
        sscanf(answer,"[ %d , %d , %ld , %d , %d ]",&buffer,&buffer2,&buffer3,&buffer4,&pl);

        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; IAMIN\n",time(NULL),i,pid,tid,dur,pl);
    } 

    unlink(ansFifoName);
    
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

    pthread_t timeThread;

    if (pthread_create(&timeThread,NULL,countTime,&nsecs) != 0)
    {
        fprintf(stderr,"Couldn't create time thread.\n");
        exit(1);
    }

    pthread_t tids[NUM_THREADS];

    struct timespec time1,time2;
    time1.tv_sec = 0;
    time1.tv_nsec = 5000000;

    int i = 0;
    while (flag)
    {
        if (pthread_create(&tids[i],NULL,threadFunc,NULL) != 0)
        {
            fprintf(stderr,"Couldn't create thread.\n");
            exit(1);
        }
        i++;
        if (nanosleep(&time1,&time2) < 0)
        {
            fprintf(stderr,"Couldn't sleep.\n");
            exit(1);
        }
    }

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    
  
    for (int i2 = 0; i2 < i; i2++) 
    {
        if (pthread_join(tids[i2],NULL) != 0)
        {
            fprintf(stderr,"Couldn't wait for thread.\n");
            exit(1);
        }
    }
    pthread_join(timeThread,NULL);
    

    pthread_mutex_destroy(&mut);
    pthread_mutex_destroy(&mutFifo);
    return 0;
}