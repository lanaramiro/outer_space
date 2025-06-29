#include "../inc/game.h"
#include "../inc/orders.h"
#include "../inc/kitchen.h"
#include "../inc/crew.h"
#include <string.h>
#include <unistd.h>

const DishInfo DISHES[] = {
    {.prep_time = 5, .cook_time = 3, .name = "Burger"},
    {.prep_time = 2, .cook_time = 1, .name = "Juice"},
    {.prep_time = 4, .cook_time = 6, .name = "Pizza"},
    {.prep_time = 3, .cook_time = 2, .name = "Salad"}
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
