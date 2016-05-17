#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include "packet.h"
#include "tui.h"
#include "list.h"
#include "misc.h"
#include "tranciver.h"


void clear_message(struct TUI_t* tui, char* input, void* argument){
    tui->line = 1;
    clear_win(tui->message_window);
}

void set_name(struct TUI_t* tui, char* input, void* argument){
    struct client_tranciver_t* tmp = (struct client_tranciver_t*)argument;
    strncpy(tmp->name, input, MAX_NAME);
}


void signal_handler(int signal_number)
{
}

int main(int argc, char *argv[]){
    struct TUI_t tui;
    struct client_tranciver_t tranciver;

    strcpy(tranciver.name, "CLIENT");

    if (signal(SIGINT, signal_handler) == SIG_ERR)
        die("Cannot handle SIGINT");

    tranciver.run = 0;

    TUI_init(&tui);
    TUI_register_command(&tui, "", "", (command_handler_t)&input_handler, &tranciver);
    TUI_register_command(&tui, "stop", "Exit SCR client", (command_handler_t)&TUI_stop, &tui);
    TUI_register_command(&tui, "connect", "Connect to remote SCR server, Ex: /connect 127.0.0.1", &connect_server, &tranciver);
    TUI_register_command(&tui, "name", "Change your name during chat", &set_name, &tranciver);
    TUI_process(&tui);
    TUI_terminate(&tui);

    if(tranciver.run){
         pthread_cancel(tranciver.reciver_thread_id);
         pthread_join(tranciver.reciver_thread_id, NULL);
    }

    return 0;
}
