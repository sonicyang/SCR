#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "packet.h"
#include "tui.h"
#include "list.h"
#include "misc.h"
#include "tranciver.h"


void clear_message(struct TUI_t* tui, char* input, void* argument){
    tui->line = 1;
    clear_win(tui->message_window);

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
    TUI_register_command(&tui, "", (command_handler_t)&input_handler, &tranciver);
    TUI_register_command(&tui, "stop", (command_handler_t)&TUI_stop, &tui);
    TUI_register_command(&tui, "clear", &clear_message, &tui);
    TUI_register_command(&tui, "connect", &connect_server, &tranciver);
    TUI_process(&tui);
    TUI_terminate(&tui);

    if(tranciver.run){
         pthread_cancel(tranciver.reciver_thread_id);
         pthread_join(tranciver.reciver_thread_id, NULL);
    }

    return 0;
}
