#include "signal_handling.h"
#include "log_file.h"
#include "calc_time.h"

void sigint_handler(int signo)
{
    if (signo != SIGINT) write(STDERR_FILENO,"This handler shouldn't have been called.\n",41);
    printLogEntry(log_filename,getInstant(),getpid(),RECV_SIGNAL,"SIGINT");
    send_signals_to_children(SIGSTOP);
    char answer[2];
    do{
        write(STDOUT_FILENO,"\nThe program has been paused. Would you like to terminate? (y/n): \n",67);
        read(STDIN_FILENO,answer,sizeof(answer));
        //printf("Answer: %s",answer);
        if (strcmp(answer,"n\n") == 0) {send_signals_to_children(SIGCONT); break;}
        else if (strcmp(answer,"y\n") == 0) {send_signals_to_children(SIGTERM); printLogEntry(log_filename,getInstant(),getpid(),EXIT,"0"); exit(0); break;}
    } while (1);
}

void sigterm_handler(int signo)
{
    if (signo != SIGTERM) write(STDERR_FILENO,"This handler shouldn't have been called.\n",41);
    printLogEntry(log_filename,getInstant(),getpid(),RECV_SIGNAL,"SIGTERM");
    exit(0);
}

void sigcont_handler(int signo)
{
    if (signo != SIGCONT) write(STDERR_FILENO,"This handler shouldn't have been called.\n",41);
    printLogEntry(log_filename,getInstant(),getpid(),RECV_SIGNAL,"SIGCONT");
}

void install_handlers()
{
    install_handler(SIGINT);
    install_handler(SIGTERM);
    install_handler(SIGCONT);
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

void send_signals_to_children(int signo)
{
    char strSigno[10] = "";
    switch(signo)
    {
        case SIGINT:  {strcpy(strSigno,"SIGINT");  break;}
        case SIGCONT: {strcpy(strSigno,"SIGCONT"); break;}
        case SIGTERM: {strcpy(strSigno,"SIGTERM"); break;}
        case SIGSTOP: {strcpy(strSigno,"SIGSTOP"); break;}
        default: {write(STDERR_FILENO,"Invalid signo value received.\n",30); break;}
    }

    for (int i = 0; i < childIndex; i++) 
    {
        kill(-firstLevelChildren[i],signo); 
        char entryContent[STR_LEN] = "";
        sprintf(entryContent,"%s %d",strSigno,-firstLevelChildren[i]);
        printLogEntry(log_filename,getInstant(),getpid(),SEND_SIGNAL,entryContent);
    }
}

void install_handler(int signo)
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    switch(signo)
    {
        case SIGINT:  {action.sa_handler = sigint_handler;  break;}
        case SIGTERM: {action.sa_handler = sigterm_handler; break;}
        case SIGCONT: {action.sa_handler = sigcont_handler; break;}
        default: {write(STDERR_FILENO,"Invalid signo value received.\n",30); break;}
    }

    sigaction(signo,&action,NULL);
}

void uninstall_handler(int signo)
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigaction(signo,&action,NULL);
}

