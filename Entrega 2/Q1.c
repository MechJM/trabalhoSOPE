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
#define STR_LEN 200

char fifoname[STR_LEN] = "";
int flag = 1;
pthread_mutex_t flagMut = PTHREAD_MUTEX_INITIALIZER;

struct message
{
    int i;
    int pid;
    long int tid;
    int dur;
    int pl;
};

void sigalrm_handler(int signo)
{
    if (signo != SIGALRM)
    {
        fprintf(stderr,"This handler shouldn't have been called.\n");
        return;
    }
    pthread_mutex_lock(&flagMut);
    flag = 0;
    pthread_mutex_unlock(&flagMut);
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
     

    printf("Cheguei aqui\n"); 
    FILE* ansFifoPtr = fopen(ansFifoName,"w");
    if (ansFifoPtr == NULL)
    {
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; GAVUP\n",time(NULL),i,pid,tid,dur,pl);
        pthread_exit(0);
    }
    printf("Cheguei aqui2\n");
    
    if (flag) pl = i;
    else pl = -1;


    fprintf(ansFifoPtr,"[ %d , %d , %ld , %d , %d ]\n",i,pid,tid,dur,pl);
    printf("Cheguei aqui3\n");
    if (pl != -1)
    {
        
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; ENTER\n",time(NULL),i,pid,tid,dur,pl);
        printf("Cheguei aqui4\n");
        usleep(dur*1000);

        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; TIMUP\n",time(NULL),i,pid,tid,dur,pl);
    }
    else
    {
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; 2LATE\n",time(NULL),i,pid,tid,dur,pl);
    }
    printf("Cheguei aqui5\n");
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
    //long int nplaces,nthreads;

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
            //nplaces = atoi(argv[i]);
        }
        else if (strcmp("-n",argv[i]) == 0)
        {
            i++;
            //nthreads = atoi(argv[i]);
        }
        else strcpy(fifoname,argv[i]);
    }

    

    if (mkfifo(fifoname,0600) < 0)
    {
        fprintf(stderr,"Couldn't create public FIFO.\n");
        exit(1);
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
    struct message reqs[NUM_THREADS];
    char request[STR_LEN] = "";

    
    int i = 0; 

    while (flag)
    {
        FILE* reqFifoPtr = fopen(fifoname,"r");
        if (reqFifoPtr == NULL) exit(1);
        while (fgets(request,STR_LEN,reqFifoPtr) != NULL)
        {
            sscanf(request,"[ %d , %d , %ld , %d , %d ]",&reqs[i].i,&reqs[i].pid,&reqs[i].tid,&reqs[i].dur,&reqs[i].pl);
            if (pthread_create(&tids[i],NULL,threadFunc,&reqs[i]) != 0)
            {
                fprintf(stderr,"Couldn't create thread.\n");
                exit(1);
            }
            i++;
            usleep(5000);
        }
        fclose(reqFifoPtr);
    }
    

    pthread_mutex_lock(&flagMut);
    flag = 0;
    pthread_mutex_unlock(&flagMut);
    unlink(fifoname);

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

    pthread_mutex_destroy(&flagMut);


    return 0;
}