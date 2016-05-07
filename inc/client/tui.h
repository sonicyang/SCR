#ifndef __TUI_H__
#define __TUI_H__

#include <curses.h>

struct TUI_t{
    int run;
    WINDOW* message_window;
    WINDOW* command_window;
};

typedef void (*command_handler_t)(void*);

void TUI_init(struct TUI_t*);
void TUI_process(struct TUI_t*);
void TUI_stop(struct TUI_t*);
void TUI_terminate(struct TUI_t*);

void TUI_register_command(char*, command_handler_t, void*);

#endif//__TUI_H__
