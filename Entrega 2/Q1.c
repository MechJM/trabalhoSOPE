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

void parse(char* str, char* is, char* pids, char* tids, char* durs)
{
    //int j = 1;

    int pl;

    sscanf(str,"[%s,%s,%s,%s,%d]\n",is,pids,tids,durs,&pl);
  
  
    /*
    while(str[j] != ';') // parses i
    {
        strcat(i, str[j]);
        j++;
    }
    j++;
    while(str[j] != ';') // parses pid
    {
        strcat(pid, str[j]);
        j++;
    }
    j++;
    while(str[j] != ';') // parses tid
    {
        strcat(tid, str[j]);
        j++;
    }
    j++;
    while(str[j] != ';') // parses dur
    {
        strcat(durs, str[j]);
        j++;
    }*/
}

void* threadFunc(void * arg)
{
    arg = arg; //without this the compiler reports an error about an unused parameter

    char str[STR_LEN] = "";

    FILE* reqFifoPtr = fopen(fifoname,"r");
    fgets(str, STR_LEN, reqFifoPtr);
    // fprintf(reqFifoPtr,"[%d,%d,%ld,%d,%d]\n",i,pid,tid,dur,pl);
    printf("%s",str);

    //char* i, *pid, *tid, *durs;
    char i[STR_LEN] = "";
    char pid[STR_LEN] = "";
    char tid[STR_LEN] = "";
    char durs[STR_LEN] = "";
    parse(str,i,pid,tid,durs);
    int dur = atoi(durs);
   


    printf("%ld ; %s ; %s ; %s ; %d ; %s ; RECVD\n",time(NULL),i,pid,tid,dur,i);
    fclose(reqFifoPtr);

    char ansFifoName[STR_LEN] = "";
    int pid_num = atoi(pid);
    long int tid_num = atol(tid);
    sprintf(ansFifoName,"/tmp/%d.%ld",pid_num,tid_num);
    

    FILE* ansFifoPtr = fopen(ansFifoName,"w");
    if(ansFifoPtr == NULL)
    {
        printf("%ld ; %s ; %s ; %s ; %s ; %s ; GAVUP\n",time(NULL),i,pid,tid,durs,i);
    }
    else
    {
        fprintf(ansFifoPtr,"[%s,%s,%s,%d,%s]\n",i,pid,tid,dur,i);

        printf("%ld ; %s ; %s ; %s ; %d ; %s ; ENTER\n",time(NULL),i,pid,tid,dur,i);

        usleep(dur*1000);

        printf("%ld ; %s ; %s ; %s ; %d ; %s ; TIMUP\n",time(NULL),i,pid,tid,dur,i);

        fclose(ansFifoPtr);
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

    int k = 0;
    while (flag)
    {
        if (pthread_create(&tids[k],NULL,threadFunc,NULL) != 0)
        {
            fprintf(stderr,"Couldn't create thread.\n");
            exit(1);
        }
        k++;
        usleep(5000);
    }

    char str[STR_LEN] = "";
    //char* i, *pid, *tid, *durs;
    char i[STR_LEN] = "";
    char pid[STR_LEN] = "";
    char tid[STR_LEN] = "";
    char durs[STR_LEN] = "";

    FILE* fifoPtr =  fopen(fifoname,"r");

    while(1)
    {
        fgets(str, STR_LEN, fifoPtr);
        if (str == NULL)
            break;
        parse(str,i,pid,tid,durs);
        printf("%ld ; %s ; %s ; %s ; %s ; %s ; 2LATE\n",time(NULL),i,pid,tid,durs,i);
        
        char ansFifoName[STR_LEN] = "";
        int pid_num = atoi(pid);
        long int tid_num = atol(tid);

        sprintf(ansFifoName,"/tmp/%d.%ld",pid_num,tid_num);
        FILE* ansFifoPtr = fopen(ansFifoName,"w");
        if(ansFifoPtr == NULL)
        {
            printf("%ld ; %s ; %s ; %s ; %s ; %s ; GAVUP\n",time(NULL),i,pid,tid,durs,i);
        }
        else
        {
            fprintf(ansFifoPtr,"[%s,%s,%s,%d,%d]\n",i,pid,tid,-1,-1);
            fclose(ansFifoPtr);
        }
    }
    
    for (int i2 = 0; i2 < k; i2++) pthread_join(tids[i2],NULL);
    
    return 0;
}