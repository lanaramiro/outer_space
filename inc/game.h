#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ORDERS 20
#define MAX_CREW 5
#define MAX_WORKSTATIONS 3
#define MAX_KITCHENS 2
#define GAME_DURATION 300 

typedef enum {
    BANANA_SPLIT,
    COFEE,
    FRIED_CHICKEN,
    RAMEN,
} DishType;

typedef struct {
    int prep_time;
    int cook_time;
    char name[20];
} DishInfo;

extern const DishInfo DISHES[];

typedef struct {
    bool is_running;
    int time_remaining;
    pthread_mutex_t game_mutex;
    pthread_cond_t game_cond;
} GameState;

extern GameState game_state;
extern WINDOW *main_win;

void init_game(void);
void cleanup_game(void);
void update_display(void);
void* game_timer_thread(void* arg);
void show_intro(void);

#endif
