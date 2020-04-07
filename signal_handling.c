#include "signal_handling.h"

void sigint_handler(int signo)
{
    if (signo != SIGINT) fprintf(stderr,"This handler shouldn't have been called.\n");
    pid_t groupPid = getpgid(getpid());
    kill(groupPid,SIGSTOP);
    char answer[STR_LEN];
    do{
        /*char pid[STR_LEN] = "";
        sprintf(pid,"%d",getpid());
        write(STDOUT_FILENO,"\n",1);
        write(STDOUT_FILENO,pid,sizeof(pid));
        write(STDOUT_FILENO,"\n",1);*/
        write(STDOUT_FILENO,"The program has been paused. Would you like to resume? (yes/no): \n",66);
        read(STDIN_FILENO,answer,3);
        if (strcmp(answer,"yes") == 0) {sigcont_handler(SIGCONT); break;}
        else if (strcmp(answer,"no") == 0) {sigterm_handler(SIGTERM); break;}
    } while (1);
}

void sigcont_handler(int signo)
{
    if (signo != SIGCONT) fprintf(stderr,"This handler shouldn't have been called.\n");
    pid_t groupPid = getpgid(getpid());
    kill(groupPid,SIGCONT);
}

void sigterm_handler(int signo)
{
    if (signo != SIGTERM) fprintf(stderr,"This handler shouldn't have been called.\n");
    pid_t groupPid = getpgid(getpid());
    kill(groupPid,SIGTERM);
    exit(0);
}

void install_handlers()
{
    struct sigaction action,action2,action3;

    sigemptyset(&action.sa_mask);
    //sigaddset(&action.sa_mask,SIGINT);
    action.sa_handler = sigint_handler;
    action.sa_flags = 0;
    sigaction(SIGINT,&action,NULL);

    sigemptyset(&action2.sa_mask);
    action2.sa_handler = sigcont_handler;
    action2.sa_flags = 0;
    sigaction(SIGCONT,&action2,NULL);

    sigemptyset(&action3.sa_mask);
    action3.sa_handler = sigterm_handler;
    action3.sa_flags = 0;
    sigaction(SIGTERM,&action3,NULL);
}

void uninstall_handlers()
{
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    //sigaddset(&action.sa_mask,SIGINT);
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigaction(SIGINT,&action,NULL);
}

void block_signal(int signo)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,signo);
    sigprocmask(SIG_BLOCK,&mask,NULL);
}

void unblock_signal(int signo)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,signo);
    sigprocmask(SIG_UNBLOCK,&mask,NULL);
}