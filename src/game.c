#include "../inc/game.h"
#include "../inc/orders.h"
#include "../inc/kitchen.h"
#include "../inc/crew.h"
#include <string.h>
#include <unistd.h>

const DishInfo DISHES[] = {
    {.prep_time = 5, .cook_time = 3, .name = "Banana Split"},
    {.prep_time = 2, .cook_time = 1, .name = "Cofee"},
    {.prep_time = 4, .cook_time = 6, .name = "Fried Chicken"},
    {.prep_time = 3, .cook_time = 2, .name = "Ramen"}
};

GameState game_state;
WINDOW *main_win;

void init_game(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    
    main_win = newwin(LINES, COLS, 0, 0);
    keypad(main_win, TRUE);
    nodelay(main_win, TRUE);
    
    game_state.is_running = true;
    game_state.time_remaining = GAME_DURATION;
    pthread_mutex_init(&game_state.game_mutex, NULL);
    pthread_cond_init(&game_state.game_cond, NULL);
    
    init_orders();
    init_kitchen();
    init_crew();
}

void cleanup_game(void) {
    cleanup_crew();
    cleanup_kitchen();
    cleanup_orders();
    
    pthread_mutex_destroy(&game_state.game_mutex);
    pthread_cond_destroy(&game_state.game_cond);
    
    delwin(main_win);
    endwin();
}

void update_display(void) {
    werase(main_win);
    box(main_win, 0, 0);
    
    mvwprintw(main_win, 1, 2, "Time remaining: %d seconds", game_state.time_remaining);
    
    mvwprintw(main_win, 3, 2, "Pending Orders:");
    OrderNode* current = order_list.head;
    int y = 4;
    while (current != NULL && y < LINES-2) {
        if (!current->is_assigned) {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, y++, 4, "[%d] %s", current->order_id, DISHES[current->dish].name);
            wattroff(main_win, COLOR_PAIR(2));
        }
        current = current->next;
    }
    
    mvwprintw(main_win, 3, 40, "Crew Status:");
    for (int i = 0; i < MAX_CREW; i++) {
        const char* state_str;
        int color;
        
        switch (crew.members[i].state) {
            case IDLE:
                state_str = "IDLE";
                color = 1;
                break;
            case PREPARING:
                state_str = "PREPARING";
                color = 2;
                break;
            case COOKING:
                state_str = "COOKING";
                color = 3;
                break;
        }
        
        wattron(main_win, COLOR_PAIR(color));
        mvwprintw(main_win, 4 + i, 42, "Crew %d: %s", i + 1, state_str);
        wattroff(main_win, COLOR_PAIR(color));
    }
    
    wrefresh(main_win);
}

void* game_timer_thread(void* arg) {
    while (game_state.is_running && game_state.time_remaining > 0) {
        sleep(1);
        pthread_mutex_lock(&game_state.game_mutex);
        game_state.time_remaining--;
        pthread_mutex_unlock(&game_state.game_mutex);
        pthread_cond_broadcast(&game_state.game_cond);
    }
    
    game_state.is_running = false;
    return NULL;
}

void show_intro(void) {
    werase(main_win);
    box(main_win, 0, 0);
    int y = 2;
    mvwprintw(main_win, y++, 40, "+*-_+* WELCOME TO OUTER SPACE! *+_-*+");
    y++;
    mvwprintw(main_win, y++, 4, "Context:");
    mvwprintw(main_win, y++, 6, "You are a chef on board of a spacial restaurant.");
    mvwprintw(main_win, y++, 6, "This restaraunt is the most famous of all galaxys. All because of the great founder Lana!");
    mvwprintw(main_win, y++, 6, "She made the most incredible menu ever:");
    mvwprintw(main_win, y++, 8, "* Banana Split (b)");
    mvwprintw(main_win, y++, 8, "* Cofee (c)");
    mvwprintw(main_win, y++, 8, "* Fried Chicken (f)");
    mvwprintw(main_win, y++, 8, "* Ramen (r)");
    y++;
    mvwprintw(main_win, y++, 4, "Instructions:");
    mvwprintw(main_win, y++, 6, "Your work is to ask the crew to make the orders. Simple as that.");
    mvwprintw(main_win, y++, 6, "The only thing is, a crew member can't make more than one order at the same time.");
    mvwprintw(main_win, y++, 6, "Just cause we're in a spaceship, it doesn't mean the crew members are robots! Not yet at least...");
    mvwprintw(main_win, y++, 6, "To make a order, type the crew member number and the letter of the order he must do.");
    mvwprintw(main_win, y++, 6, "(ex: 1b for member 1 make a banana split)");
    mvwprintw(main_win, y++, 6, "Good Luck!");
    y++;
    mvwprintw(main_win, y++, 4, "Press any key to start...");
    wrefresh(main_win);
    nodelay(main_win, FALSE);
    wgetch(main_win);
    nodelay(main_win, TRUE);
}
