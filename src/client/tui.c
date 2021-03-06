#include <curses.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>


#include "tui.h"
#include "list.h"
#include "misc.h"

#define INPUT_BUFFER_SIZE 128

static WINDOW *create_newwin(int, int, int, int);
static void destroy_win(WINDOW*);

static void TUI_parse_command(struct TUI_t*, char*);
static void TUI_refresh(struct TUI_t*);
void clear_win(WINDOW* local_win);

void TUI_init(struct TUI_t* tui){
    tui->command_chain = create_list(sizeof(struct registered_command_t));
    tui->default_input_callback.handler = NULL;
    tui->line = 0;

    sem_unlink("APRINT_SEM");
    tui->print = sem_open("APRINT_SEM", O_CREAT|O_EXCL, S_IRWXU, 0);

    initscr();
    cbreak();
    noecho();

    tui->message_window = create_newwin(LINES - 3, COLS, 0, 0);
    tui->command_window = create_newwin(3, COLS, LINES - 3, 0);

    wtimeout(tui->command_window, 30);
    keypad(tui->command_window, TRUE);

    tui->message_list = create_list(sizeof(struct message_t));
    //Dummy Head
    tui->print_start = list_allocate(tui->message_list);
    init_message(tui->print_start->data, "", 0);
}

void TUI_process(struct TUI_t* tui){
    int i;
    int input_buffer;
    char input[INPUT_BUFFER_SIZE + 1];

    tui->run = 1;
    TUI_prompt_welcome(tui);

    wmove(tui->command_window, 1, 1);
    i = 0;
    while(tui->run){
        input_buffer = wgetch(tui->command_window);
        switch(input_buffer){
            case ERR:
                break;
            case KEY_BACKSPACE:
            case KEY_DC:
            case 127:
                if(i > 0){
                    wmove(tui->command_window, 1, i);
                    waddch(tui->command_window, ' ');
                    wmove(tui->command_window, 1, i);
                    i--;
                }
                break;
            case KEY_ENTER:
            case '\n':
                if(i){
                    clear_win(tui->command_window);
                    wmove(tui->command_window, 1, 1);
                    input[i] = '\0';
                    i = 0;
                    TUI_parse_command(tui, input);
                }
                break;
            case 32 ... 126:
                if(i < INPUT_BUFFER_SIZE){
                    waddch(tui->command_window, input_buffer);
                    input[i] = 0x000000ff & input_buffer;
                    i++;
                }
                break;

        }

        if(!sem_trywait(tui->print)){
            TUI_refresh(tui);
        }
    }
}

void TUI_write_message(struct TUI_t* tui, struct message_t* message_in){
    struct list_element_t* message;
    if(tui->run){
        message = list_allocate(tui->message_list);
        memcpy(message->data, message_in, sizeof(struct message_t));
        sem_post(tui->print);
    }
    return;
}

void TUI_error(struct TUI_t* tui, char* str){
    struct message_t message;

    init_message(&message, "ERROR", strlen(str));
    strcpy(message.buffer, str);
    TUI_write_message(tui, &message);
    if(errno){
        init_message(&message, "ERROR", strlen(strerror(errno)));
        strcpy(message.buffer, strerror(errno));
        TUI_write_message(tui, &message);
        errno = 0;
    }

    return;
}

void TUI_print(struct TUI_t* tui, char* str){
    struct message_t message;

    init_message(&message, "SYSTEM", strlen(str));
    strcpy(message.buffer, str);
    TUI_write_message(tui, &message);

    return;

}


void TUI_stop(struct TUI_t* tui){
     tui->run = 0;
     return;
}

void TUI_terminate(struct TUI_t* tui){
    sem_close(tui->print);
    sem_unlink("PRINT_SEM");
    delete_list(tui->message_list);
    endwin();
}

void TUI_register_command(struct TUI_t* tui, char* command, char* help, command_handler_t handler, void* argument){
    struct registered_command_t* tmp;
    if(strlen(command)){
        tmp = list_allocate(tui->command_chain)->data;
    }else{
        tmp = &tui->default_input_callback;
    }
    strncpy(tmp->command, command, 63);
    tmp->command[63] = '\0';

    strncpy(tmp->help, help, 127);
    tmp->help[127] = '\0';

    tmp->handler = handler;
    tmp->argument = argument;
    return;
}

void clear_win(WINDOW* local_win){
    wclear(local_win);
    box(local_win, 0 , 0);
    wrefresh(local_win);
}

void TUI_prompt_welcome(struct TUI_t* tui){
    struct list_element_t* ptr = tui->command_chain->head;
    struct registered_command_t* tmp;
    char buf[255];

    TUI_print(tui, "Welcome to SCR client!");
    TUI_print(tui, "List of command Supported by SCR client");

    while(ptr != NULL){
        tmp = (struct registered_command_t*)ptr->data;
        if(strlen(tmp->command)){
            buf[0] = '/';
            buf[1] = '\0';
            strcat(buf, tmp->command);
            TUI_print(tui, buf);

            buf[0] = ' ';
            buf[1] = ' ';
            buf[2] = ' ';
            buf[3] = ' ';
            buf[4] = '\0';
            strcat(buf, tmp->help);
            TUI_print(tui, buf);
        }
        ptr = ptr->next;
    }
    return;
}

static void TUI_refresh(struct TUI_t* tui){
    struct list_element_t* ptr;
    int i, j;
    char now[32];
    struct tm tim;

    box(tui->message_window, 0 , 0);

    ptr = tui->message_list->tail;
    for(i = 1; i < LINES - 3 - 2; i++){
        if(ptr->prev != NULL){
             ptr = ptr->prev;
        }else{
            break;
        }
    }
    i = 1;
    while(ptr != NULL){
        localtime_r((time_t*)&((struct message_t*)ptr->data)->timestamp, &tim);
        strftime(now, 30, "%X", &tim);
        wmove(tui->message_window, i, 1);
        for(j = 0; j < COLS - 2; j++)
            wprintw(tui->message_window, " ");
        mvwprintw(tui->message_window, i, 1, now);
        mvwprintw(tui->message_window, i, 12, ((struct message_t*)ptr->data)->sender);
        mvwprintw(tui->message_window, i, 30, ((struct message_t*)ptr->data)->buffer);
        i++;
        ptr = ptr->next;
    }

    wrefresh(tui->message_window);
}

static void TUI_parse_command(struct TUI_t* tui, char* input){
    struct list_element_t* ptr;
    struct registered_command_t* tmp;

    if(input[0] == '/' && tui->command_chain->size > 0){
        ptr = tui->command_chain->head;
        while(ptr != NULL){
            tmp = ((struct registered_command_t*)ptr->data);

            if(!strncmp(tmp->command, input + 1, strlen(tmp->command))){
                (*(tmp->handler))(tui, input + strlen(tmp->command) + 2, tmp->argument);
            }
            ptr = ptr->next;
        }
    }else{
        if(tui->default_input_callback.handler)
             (*(tui->default_input_callback.handler))(tui, input, tui->default_input_callback.argument);
    }
    return;
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

