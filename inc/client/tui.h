#ifndef __TUI_H__
#define __TUI_H__

#include <curses.h>

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
    command_handler_t default_input_callback;
    WINDOW* message_window;
    WINDOW* command_window;
};

void TUI_init(struct TUI_t*, command_handler_t);
void TUI_process(struct TUI_t*);
void TUI_stop(struct TUI_t*);
void TUI_terminate(struct TUI_t*);

void TUI_register_command(struct TUI_t*, char*, command_handler_t, void*);

#endif//__TUI_H__
