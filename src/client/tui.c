#include <curses.h>

#include "tui.h"

static WINDOW *create_newwin(int, int, int, int);
static void destroy_win(WINDOW*);

static void TUI_prompt_welcome(struct TUI_t*);

void TUI_init(struct TUI_t* tui){
    initscr();
    cbreak();

    tui->message_window = create_newwin(LINES - 3, COLS, 0, 0);
    tui->command_window = create_newwin(3, COLS, LINES - 3, 0);

    keypad(tui->command_window, TRUE);

    TUI_prompt_welcome(tui);
}

void TUI_process(struct TUI_t* tui){
    int ch;

    while((ch = wgetch(tui->command_window)) != KEY_F(1))
    {

    }
}

void TUI_terminate(struct TUI_t* tui){
    endwin();
}

static void TUI_prompt_welcome(struct TUI_t* tui){
    mvwprintw(tui->message_window, 1, 1, "Press F1 to exit");
    wrefresh(tui->message_window);
}

static WINDOW *create_newwin(int height, int width, int starty, int startx){
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
}

static void destroy_win(WINDOW *local_win){
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}
