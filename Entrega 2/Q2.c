#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>

#include "queue.h"

#define NUM_THREADS 100000
#define NUM_PLACES 100000
#define STR_LEN 200
#define TSHARED 0

struct message
{
    int i;
    int pid;
    long int tid;
    int dur;
    int pl;
};


char fifoname[STR_LEN] = "";
int flag = 1;
int needCleanup = 1;
pthread_mutex_t flagMut = PTHREAD_MUTEX_INITIALIZER;
int threadsCreated = 0;
pthread_t tids[NUM_THREADS];
sem_t bathroomPlace;
int places[NUM_PLACES];
long int nplaces = 0,nthreads = 0;
pthread_mutex_t placeMut = PTHREAD_MUTEX_INITIALIZER;
struct message reqs[NUM_THREADS];
pthread_mutex_t queueMut = PTHREAD_MUTEX_INITIALIZER;


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

    time1.tv_nsec = 5000000;
    time1.tv_sec = 0;

    if (nanosleep(&time1,&time2) < 0)
    {
        fprintf(stderr,"Couldn't sleep.\n");
        pthread_exit(0);
    }

    if (needCleanup)
    {
        unlink(fifoname);

        sigset_t mask;
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        
        for (int i2 = 0; i2 < threadsCreated; i2++) 
        {
            if (pthread_join(tids[i2],NULL) != 0)
            {
                fprintf(stderr,"Couldn't wait for thread.\n");
                exit(1);
            }
        }

        pthread_mutex_destroy(&flagMut);
        pthread_mutex_destroy(&placeMut);
        pthread_mutex_destroy(&queueMut);
        sem_destroy(&bathroomPlace);
        
        exit(0);
    }

    return NULL;
}

void* threadFunc(void* arg)
{
    arg=arg;//otherwise the compiler reports an error about unused parameters
    int size = 1;
    while (size != 0 || flag) 
    {
        int reqIndex;
        pthread_mutex_lock(&queueMut);
        reqIndex = queuePop();
        pthread_mutex_unlock(&queueMut);

        if (reqIndex == -1) continue;

        int i = reqs[reqIndex].i;
        int pid = reqs[reqIndex].pid;
        long int tid = reqs[reqIndex].tid;
        int dur = reqs[reqIndex].dur;
        int pl = reqs[reqIndex].pl;
    
        char ansFifoName[STR_LEN] = "";
        sprintf(ansFifoName,"/tmp/%d.%ld",pid,tid);

        pid = getpid();
        tid = pthread_self();

        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; RECVD\n",time(NULL),i,pid,tid,dur,pl);
        
        FILE* ansFifoPtr = fopen(ansFifoName,"w");
        if (ansFifoPtr == NULL)
        {
            printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; GAVUP\n",time(NULL),i,pid,tid,dur,pl);
            pthread_exit(0);
        }
        
        
        if (flag)
        {
            sem_wait(&bathroomPlace);
            
            for (int i = 0; i < nplaces; i++)
            {
                pthread_mutex_lock(&placeMut);
                
                if (places[i] == 0)
                {
                    places[i] = 1;
                    pthread_mutex_unlock(&placeMut);
                    pl = (i + 1);
                    break;
                }
                pthread_mutex_unlock(&placeMut);
            }
            
        } 
        else pl = -1;

        
        if (fprintf(ansFifoPtr,"[ %d , %d , %ld , %d , %d ]\n",i,pid,tid,dur,pl) < 0)
        {
            fclose(ansFifoPtr);
            printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; GAVUP\n",time(NULL),i,pid,tid,dur,pl);
            pthread_exit(0);
        }

        struct timespec time1,time2;

        if (pl != -1)
        {
            
            printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; ENTER\n",time(NULL),i,pid,tid,dur,pl);
            
            
            time1.tv_sec = 0;
            time1.tv_nsec = dur * 1000000;
            
            if (nanosleep(&time1,&time2) < 0)
            {
                fprintf(stderr,"Couldn't sleep.\n");
                pthread_exit(0);
            }

            pthread_mutex_lock(&placeMut);
            places[pl-1] = 0;
            pthread_mutex_unlock(&placeMut);
            sem_post(&bathroomPlace);

            printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; TIMUP\n",time(NULL),i,pid,tid,dur,pl);
        }
        else
        {
            printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; 2LATE\n",time(NULL),i,pid,tid,dur,pl);
        }
        
        fclose(ansFifoPtr);

        
        /*
        time1.tv_nsec = 5000000;
        time1.tv_sec = 0;

        if (nanosleep(&time1,&time2) < 0)
        {
            fprintf(stderr,"Couldn't sleep.\n");
            pthread_exit(0);
        }*/
        

        pthread_mutex_lock(&queueMut);
        size = queueSize();
        pthread_mutex_unlock(&queueMut);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"Usage: Q1 <-t nsecs> fifoname\n");
        exit(1);
    }

    int nsecs;

    for (int i = 1; argv[i] != NULL; i++)
    {
        if (strcmp("-t",argv[i]) == 0)
        {
            i++;
            nsecs = atoi(argv[i]);
        }
        else if (strcmp("-l",argv[i]) == 0)
        {
            i++;
            nplaces = atoi(argv[i]);
        }
        else if (strcmp("-n",argv[i]) == 0)
        {
            i++;
            nthreads = atoi(argv[i]);
        }
        else strcpy(fifoname,argv[i]);
    }

    for (int i = 0; i < nplaces; i++) places[i] = 0;

    initQueue();

    if (nplaces != 0) sem_init(&bathroomPlace,TSHARED,nplaces);
    else sem_init(&bathroomPlace,TSHARED,NUM_THREADS);

    if (mkfifo(fifoname,0600) < 0)
    {
        fprintf(stderr,"Couldn't create public FIFO.\n");
        exit(1);
    }

    pthread_t timeThread;

    if (pthread_create(&timeThread,NULL,countTime,&nsecs) != 0)
    {
        fprintf(stderr,"Couldn't create time thread.\n");
        exit(1);
    }
    
    
    char request[STR_LEN] = "";
    
    int requestIndex = 0;

    if (nthreads == 0) nthreads = NUM_THREADS;

    while (flag)
    {
        
        FILE* reqFifoPtr = fopen(fifoname,"r");
        if (reqFifoPtr == NULL)
        {
            fprintf(stderr,"Couldn't open public FIFO. Error value: %d\n'",errno);
            exit(1);
        }
        while (fgets(request,STR_LEN,reqFifoPtr) != NULL)
        {
            sscanf(request,"[ %d , %d , %ld , %d , %d ]",&reqs[requestIndex].i,&reqs[requestIndex].pid,&reqs[requestIndex].tid,&reqs[requestIndex].dur,&reqs[requestIndex].pl);
            pthread_mutex_lock(&queueMut);
            queuePush(requestIndex);
            pthread_mutex_unlock(&queueMut);
            requestIndex++;
            if (threadsCreated < nthreads)
            {
                
                if (pthread_create(&tids[threadsCreated],NULL,threadFunc,NULL) != 0)
                    {
                        fprintf(stderr,"Couldn't create thread.\n");
                        exit(1);
                    }
                    threadsCreated++;
            } 
        }
        
        fclose(reqFifoPtr);
    }
    
    needCleanup = 0;
    
    unlink(fifoname);

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    for (int i2 = 0; i2 < threadsCreated; i2++) 
    {
        if (pthread_join(tids[i2],NULL) != 0)
        {
            fprintf(stderr,"Couldn't wait for thread.\n");
            exit(1);
        }
    }
    pthread_join(timeThread,NULL);


    pthread_mutex_destroy(&flagMut);
    pthread_mutex_destroy(&placeMut);
    pthread_mutex_destroy(&queueMut);
    sem_destroy(&bathroomPlace);

    return 0;
}