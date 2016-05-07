#include <curses.h>
#include <string.h>

#include "tui.h"
#include "list.h"

struct registered_command_t{
    command_handler_t handler;
    void* argument;
    char command[64];
};


static WINDOW *create_newwin(int, int, int, int);
static void destroy_win(WINDOW*);
static void clear_win(WINDOW*);

static void TUI_prompt_welcome(struct TUI_t*);

void TUI_init(struct TUI_t* tui){
    tui->command_chain = create_list(sizeof(struct registered_command_t));
    tui->default_input_callback = NULL;

    initscr();
    cbreak();

    tui->message_window = create_newwin(LINES - 3, COLS, 0, 0);
    tui->command_window = create_newwin(3, COLS, LINES - 3, 0);

    keypad(tui->command_window, TRUE);

    TUI_prompt_welcome(tui);
}

void TUI_process(struct TUI_t* tui){
    int ch;
    char input[128];
    struct list_element_t* ptr;
    struct registered_command_t* tmp;

    tui->run = 1;
    while(tui->run){
        wmove(tui->command_window, 1, 1);
        wgetstr(tui->command_window, input);

        if(input[0] == '/' && tui->command_chain->size > 0){
            ptr = tui->command_chain->head;
            while(ptr != NULL){
                tmp = ((struct registered_command_t*)ptr->data);

                if(!strcmp(tmp->command, input + 1)){
                    (*(tmp->handler))(tmp->argument);
                }
                ptr = ptr->next;
            }
        }else{
            if(tui->default_input_callback)
                 (*(tui->default_input_callback))("123");
        }

        wprintw(tui->message_window, input);

        clear_win(tui->command_window);
        wrefresh(tui->message_window);
        wrefresh(tui->command_window);
    }
}

void TUI_stop(struct TUI_t* tui){
     tui->run = 0;
     return;
}

void TUI_terminate(struct TUI_t* tui){
    endwin();
}

void TUI_register_command(struct TUI_t* tui, char* command, command_handler_t handler, void* argument){
    struct registered_command_t* tmp = list_allocate(tui->command_chain)->data;
    strncpy(tmp->command, command, 63);
    tmp->command[63] = '\0';

    tmp->handler = handler;
    tmp->argument = argument;
    return;
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

void clear_win(WINDOW* local_win){
    wclear(local_win);
    box(local_win, 0 , 0);
    wrefresh(local_win);
}
