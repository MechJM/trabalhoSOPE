#include "signal_handling.h"

void sigint_handler(int signo)
{
    if (signo != SIGINT) fprintf(stderr,"This handler shouldn't have been called.\n");
    send_signals_to_children(SIGSTOP);
    sleep(1);
    char answer[5];
    do{
        write(STDOUT_FILENO,"The program has been paused. Would you like to resume? (yes/no): \n",66);
        read(STDIN_FILENO,answer,4);
        printf("Answer: %s",answer);
        if (strcmp(answer,"yes\n") == 0) {send_signals_to_children(SIGCONT); break;}
        else if (strcmp(answer,"no\n") == 0) {send_signals_to_children(SIGTERM); exit(0); break;}
    } while (1);
}

void install_handlers()
{
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = sigint_handler;
    action.sa_flags = 0;
    sigaction(SIGINT,&action,NULL);

}

void uninstall_handlers()
{
    struct sigaction action;

    sigemptyset(&action.sa_mask);
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

void send_signals_to_children(int signo)
{
    for (int i = 0; i < childIndex; i++) kill(firstLevelChildren[i],signo);
}