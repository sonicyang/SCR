#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>

#include "listener.h"
#include "client_handler.h"
#include "misc.h"
#include "setting.h"

#define OPTION "f:"

struct listener_t listener_instance;

void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
        pthread_cancel(listener_instance.thread_id);
}

int main(int argc, char *argv[]){
    int cmd_opt = 0;
    char setting_fn[64] = "\0";
    struct setting_t setting;

    if (signal(SIGINT, signal_handler) == SIG_ERR)
        die("Cannot handle SIGINT");

    while(1){
        cmd_opt = getopt(argc, argv, OPTION);
        if(cmd_opt == -1)
            break;
        switch (cmd_opt) {
            case 'f':
                strcpy(setting_fn, optarg);
                break;
            case '?':
                fprintf(stderr, "Illegal option:-%c\n", isprint(optopt)?optopt:'#');
                break;
            default:
                fprintf(stderr, "Not supported option exist\n");
                break;
        }
    }

    if(!strlen(setting_fn)){
        die("Must provide a setting file, -fsetting");
    }

    printf("Parsing Setting File....");
    parse_setting(setting_fn, &setting);
    printf("Done\n");

    printf("Creating listener_instance thread...");
    start_listener(&setting, &(listener_instance.thread_id));
    printf("Done\n");

    pthread_join(listener_instance.thread_id, NULL);

    exit(0);

}
