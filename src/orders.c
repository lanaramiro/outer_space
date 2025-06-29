#include "../inc/orders.h"
#include <unistd.h>

OrderList order_list = {NULL, 0};

void init_orders(void) {
    pthread_mutex_init(&order_list.orders_mutex, NULL);
    order_list.head = NULL;
    order_list.order_count = 0;
}

void cleanup_orders(void) {
    pthread_mutex_lock(&order_list.orders_mutex);
    
    OrderNode* current = order_list.head;
    while (current != NULL) {
        OrderNode* next = current->next;
        free(current);
        current = next;
    }
    
    pthread_mutex_unlock(&order_list.orders_mutex);
    pthread_mutex_destroy(&order_list.orders_mutex);
}

void add_order(DishType dish) {
    OrderNode* new_node = malloc(sizeof(OrderNode));
    new_node->dish = dish;
    new_node->is_assigned = false;
    new_node->order_id = order_list.order_count++;
    new_node->next = NULL;
    
    pthread_mutex_lock(&order_list.orders_mutex);
    
    if (order_list.head == NULL) {
        order_list.head = new_node;
    } else {
        OrderNode* current = order_list.head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    
    pthread_mutex_unlock(&order_list.orders_mutex);
}

void remove_order(int order_id) {
    pthread_mutex_lock(&order_list.orders_mutex);
    
    OrderNode* current = order_list.head;
    OrderNode* prev = NULL;
    
    while (current != NULL) {
        if (current->order_id == order_id) {
            if (prev == NULL) {
                order_list.head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    
    pthread_mutex_unlock(&order_list.orders_mutex);
}

void* order_generator_thread(void* arg) {
    while (game_state.is_running) {
        int delay = (rand() % 11) + 5;
        sleep(delay);
        
        if (!game_state.is_running) break;
        
        DishType dish = rand() % 4; 
        add_order(dish);
    }
    return NULL;
}
