#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H

#include "global.h"

void sigint_handler(int signo);

void sigterm_handler(int signo);

void sigcont_handler(int signo);

void install_handler(int signo);

void uninstall_handler(int signo);

void install_handlers();

void block_signal(int signo);

void unblock_signal(int signo);

void send_signals_to_children(int signo);

#endif