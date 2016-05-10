#ifndef __TUI_H__
#define __TUI_H__

#include <curses.h>
#include <semaphore.h>

#include "list.h"
#include "message.h"

struct TUI_t;

typedef void (*command_handler_t)(struct TUI_t*, char*, void*);

struct registered_command_t{
    command_handler_t handler;
    void* argument;
    char command[64];
};

struct TUI_t{
    int run;
    struct list_t* command_chain;
    int line;
    struct registered_command_t default_input_callback;
    struct list_t* message_list;
    struct list_element_t* print_start;
    sem_t* print;
    pthread_t print_thread_id;
    WINDOW* message_window;
    WINDOW* command_window;
};

void TUI_init(struct TUI_t*);
void TUI_process(struct TUI_t*);
void TUI_refresher(struct TUI_t*);
void TUI_stop(struct TUI_t*);
void TUI_terminate(struct TUI_t*);
void TUI_write_message(struct TUI_t*, struct message_t*);
void TUI_error(struct TUI_t*, char*);
void TUI_print(struct TUI_t*, char*);
void clear_win(WINDOW*);

void TUI_register_command(struct TUI_t*, char*, command_handler_t, void*);

#endif//__TUI_H__
