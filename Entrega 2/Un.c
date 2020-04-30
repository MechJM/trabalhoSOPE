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
    fprintf(reqFifoPtr,"[ %d , %d , %ld , %d , %d ]\n",i,pid,tid,dur,pl);
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
    if (fgets(answer,STR_LEN,ansFifoPtr) == NULL)
    {
        fclose(ansFifoPtr);
        printf("%ld ; %5d ; %d ; %ld ; %2d ; %5d ; FAILD\n",time(NULL),i,pid,tid,dur,pl);
        unlink(ansFifoName);
        pthread_exit(0);
    }
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

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    if ((seqNum - i) != 1) i--;
  
    for (int i2 = 0; i2 < i; i2++) 
    {
        printf("Inicio i2: %d\n",i2);
        if (pthread_join(tids[i2],NULL) != 0)
        {
            fprintf(stderr,"Couldn't wait for thread.\n");
            exit(1);
        }
        printf("Fim i2: %d\n",i2);
    }
    
    

    pthread_mutex_destroy(&mut);
    pthread_mutex_destroy(&mutFifo);
    return 0;
}