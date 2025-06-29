#ifndef CREW_H
#define CREW_H

#include "game.h"
#include "orders.h"

typedef enum {
    IDLE,
    PREPARING,
    COOKING
} CrewState;

typedef struct {
    int id;
    CrewState state;
    OrderNode* current_order;
    pthread_t thread;
} CrewMember;

typedef struct {
    CrewMember members[MAX_CREW];
    pthread_mutex_t crew_mutex;
} CrewManager;

extern CrewManager crew;

void init_crew(void);
void cleanup_crew(void);
void* crew_thread(void* arg);
bool assign_order_to_crew(int crew_id, OrderNode* order);

#endif
