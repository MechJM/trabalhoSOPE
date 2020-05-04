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

#define NUM_THREADS 100000
#define NUM_PLACES 100000
#define STR_LEN 200
#define TSHARED 0

char fifoname[STR_LEN] = "";
int flag = 1;
int needCleanup = 1;
pthread_mutex_t flagMut = PTHREAD_MUTEX_INITIALIZER;
int threadsCreated = 0;
pthread_t tids[NUM_THREADS];
sem_t bathroomPlace;
int places[NUM_PLACES];
long int nplaces = 0,nthreads = 0;

struct message
{
    int i;
    int pid;
    long int tid;
    int dur;
    int pl;
};

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
        exit(0);
    }

    return NULL;
}

void* threadFunc(void* arg)
{
    struct message messageData = (*(struct message *)arg);

    int i = messageData.i;
    int pid = messageData.pid;
    long int tid = messageData.tid;
    int dur = messageData.dur;
    int pl = messageData.pl;
   
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
            if (places[i] == 0)
            {
                places[i] = 1;
                pl = (i + 1);
                break;
            }
        }
        sem_post(&bathroomPlace);
    } 
    else pl = -1;

    
    if (fprintf(ansFifoPtr,"[ %d , %d , %ld , %d , %d ]\n",i,pid,tid,dur,pl) < 0)
    {
        fclose(ansFifoPtr);
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; GAVUP\n",time(NULL),i,pid,tid,dur,pl);
        pthread_exit(0);
    }
    
    if (pl != -1)
    {
        
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; ENTER\n",time(NULL),i,pid,tid,dur,pl);
        
        struct timespec time1,time2;
        time1.tv_sec = 0;
        time1.tv_nsec = dur * 1000000;
        
        if (nanosleep(&time1,&time2) < 0)
        {
            fprintf(stderr,"Couldn't sleep.\n");
            pthread_exit(0);
        }

        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; TIMUP\n",time(NULL),i,pid,tid,dur,pl);
    }
    else
    {
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; 2LATE\n",time(NULL),i,pid,tid,dur,pl);
    }
    
    fclose(ansFifoPtr);

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

    if (nplaces != 0) sem_init(&bathroomPlace,TSHARED,nplaces);

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
    

    
    struct message reqs[NUM_THREADS];
    char request[STR_LEN] = "";

    
    

    while (flag)
    {
        
        FILE* reqFifoPtr = fopen(fifoname,"r");
        if (reqFifoPtr == NULL)
        {
            fprintf(stderr,"Couldn't open public FIFO. Error value: %d\n'",errno);
            exit(1);
        }
        if (threadsCreated < nthreads)
        {
            while (fgets(request,STR_LEN,reqFifoPtr) != NULL)
            {
                sscanf(request,"[ %d , %d , %ld , %d , %d ]",&reqs[threadsCreated].i,&reqs[threadsCreated].pid,&reqs[threadsCreated].tid,&reqs[threadsCreated].dur,&reqs[threadsCreated].pl);
                if (pthread_create(&tids[threadsCreated],NULL,threadFunc,&reqs[threadsCreated]) != 0)
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
    sem_destroy(&bathroomPlace);

    return 0;
}