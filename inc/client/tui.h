#ifndef __TUI_H__
#define __TUI_H__

#include <curses.h>

struct TUI_t{
    WINDOW* message_window;
    WINDOW* command_window;
};

void TUI_init(struct TUI_t*);
void TUI_process(struct TUI_t*);
void TUI_terminate(struct TUI_t*);

#endif//__TUI_H__
