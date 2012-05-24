#ifndef B_SIGNAL_H
#define B_SIGNAL_H

#include <boiler/config.h>
#include <boiler/string.h>
#include <boiler/exceptions.h>
#include <boiler/globals.h>
#include <signal.h>

namespace boiler {

void sigaction_free_logs(int);
void sigaction_respawn(int);
void sigaction_terminate(int);

void signal_handling_initiation();
void signal_set_action(int signal_number, void (*signal_handler)(int));

void signal_block_in_thread(bool do_block); // true - do block, false - do unblock

void signal_interrupt(int signal_number, bool interrupt);
void signal_notification_wait();

}

#endif
