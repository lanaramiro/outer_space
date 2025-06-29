#include "../inc/kitchen.h"

KitchenState kitchen;

void init_kitchen(void) {
    pthread_mutex_init(&kitchen.kitchen_mutex, NULL);
    
    for (int i = 0; i < MAX_WORKSTATIONS; i++) {
        kitchen.workstations[i].is_occupied = false;
        kitchen.workstations[i].crew_id = -1;
    }
    
    for (int i = 0; i < MAX_KITCHENS; i++) {
        kitchen.kitchens[i].is_occupied = false;
        kitchen.kitchens[i].crew_id = -1;
    }
}

void cleanup_kitchen(void) {
    pthread_mutex_destroy(&kitchen.kitchen_mutex);
}

bool assign_workstation(int crew_id) {
    bool success = false;
    pthread_mutex_lock(&kitchen.kitchen_mutex);
    
    for (int i = 0; i < MAX_WORKSTATIONS; i++) {
        if (!kitchen.workstations[i].is_occupied) {
            kitchen.workstations[i].is_occupied = true;
            kitchen.workstations[i].crew_id = crew_id;
            success = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&kitchen.kitchen_mutex);
    return success;
}

void release_workstation(int crew_id) {
    pthread_mutex_lock(&kitchen.kitchen_mutex);
    
    for (int i = 0; i < MAX_WORKSTATIONS; i++) {
        if (kitchen.workstations[i].crew_id == crew_id) {
            kitchen.workstations[i].is_occupied = false;
            kitchen.workstations[i].crew_id = -1;
            break;
        }
    }
    
    pthread_mutex_unlock(&kitchen.kitchen_mutex);
}

bool assign_kitchen(int crew_id) {
    bool success = false;
    pthread_mutex_lock(&kitchen.kitchen_mutex);
    
    for (int i = 0; i < MAX_KITCHENS; i++) {
        if (!kitchen.kitchens[i].is_occupied) {
            kitchen.kitchens[i].is_occupied = true;
            kitchen.kitchens[i].crew_id = crew_id;
            success = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&kitchen.kitchen_mutex);
    return success;
}

void release_kitchen(int crew_id) {
    pthread_mutex_lock(&kitchen.kitchen_mutex);
    
    for (int i = 0; i < MAX_KITCHENS; i++) {
        if (kitchen.kitchens[i].crew_id == crew_id) {
            kitchen.kitchens[i].is_occupied = false;
            kitchen.kitchens[i].crew_id = -1;
            break;
        }
    }
    
    pthread_mutex_unlock(&kitchen.kitchen_mutex);
}
