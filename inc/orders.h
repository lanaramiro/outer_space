#ifndef ORDERS_H
#define ORDERS_H

#include "game.h"

typedef struct OrderNode {
    DishType dish;
    int order_id;
    bool is_assigned;
    struct OrderNode* next;
} OrderNode;

typedef struct {
    OrderNode* head;
    int order_count;
    pthread_mutex_t orders_mutex;
} OrderList;

extern OrderList order_list;

void init_orders(void);
void cleanup_orders(void);
void add_order(DishType dish);
void remove_order(int order_id);
void* order_generator_thread(void* arg);

#endif
