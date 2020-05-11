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
#define DONT_EXECUTE 0
#define STR_LEN 100

struct logInfo 
{
    int i;
    int pid;
    long int tid;
    int dur;
    int pl;
    char oper[STR_LEN];
};

void sigusr1_handler(int signo){if (signo != SIGUSR1) fprintf(stderr,"This handler shouldn't have been called.\n");}
void sigusr2_handler(int signo){if (signo != SIGUSR2) fprintf(stderr,"This handler shouldn't have been called.\n"); pthread_exit(0);}

char fifoname[STR_LEN] = "";
int seqNum = 1;
int flag = 1;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutFifo = PTHREAD_MUTEX_INITIALIZER;
int needCleanup[NUM_THREADS];

void * countTime(void * arg)
{
    int time = *((int * )arg);

    struct timespec time1,time2;

    time1.tv_sec = time;
    time1.tv_nsec = 0;

    nanosleep(&time1,&time2);
    flag = 0;
    return NULL;
}



void* threadFunc(void * arg)
{
    int index = *(int *)arg;

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sigusr2_handler;
    if (sigaction(SIGUSR2,&action,NULL) < 0)
    {
        fprintf(stderr,"Couldn't install handler.\n");
        pthread_exit(0);
    }

    pthread_mutex_lock(&mut);
    int i = seqNum++;
    pthread_mutex_unlock(&mut);

    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    int dur = rand() % 20 + 1;
    int pl = -1;

    struct logInfo log;
    

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
        fclose(reqFifoPtr);
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
    
    log.i = i;
    log.pid = pid;
    log.tid = tid;
    log.dur = dur;
    log.pl = pl;
    strcpy(log.oper,"FAILD");
   
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

    needCleanup[index] = 0;
    
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"Usage: U1 <-t nsecs> fifoname\n");
        exit(1);
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sigusr1_handler;
    if (sigaction(SIGUSR1,&action,NULL) < 0)
    {
        fprintf(stderr,"Couldn't install handler.\n");
        exit(1);
    }

    srand(time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) needCleanup[i] = 1;

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

    int indexes[NUM_THREADS];

    int i = 0;
    while (flag)
    {
        indexes[i] = i;
        if (pthread_create(&tids[i],NULL,threadFunc,&indexes[i]) != 0)
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

  
    for (int i2 = 0; i2 < i; i2++) 
    {
        if (needCleanup[i2]) pthread_kill(tids[i2],SIGUSR2);
        if (pthread_join(tids[i2],NULL) != 0)
        {
            fprintf(stderr,"Couldn't wait for thread.\n");
            exit(1);
        }
    }

    pthread_kill(timeThread,SIGUSR1);
    pthread_join(timeThread,NULL);
    

    pthread_mutex_destroy(&mut);
    pthread_mutex_destroy(&mutFifo);
    return 0;
}