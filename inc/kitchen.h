#ifndef KITCHEN_H
#define KITCHEN_H

#include "game.h"

typedef struct {
    bool is_occupied;
    int crew_id;
} Station;

typedef struct {
    Station workstations[MAX_WORKSTATIONS];
    Station kitchens[MAX_KITCHENS];
    pthread_mutex_t kitchen_mutex;
} KitchenState;

extern KitchenState kitchen;

void init_kitchen(void);
void cleanup_kitchen(void);
bool assign_workstation(int crew_id);
void release_workstation(int crew_id);
bool assign_kitchen(int crew_id);
void release_kitchen(int crew_id);

#endif
