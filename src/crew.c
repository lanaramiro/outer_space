#include "../inc/crew.h"
#include "../inc/kitchen.h"
#include <unistd.h>

CrewManager crew;

void init_crew(void) {
    pthread_mutex_init(&crew.crew_mutex, NULL);
    
    for (int i = 0; i < MAX_CREW; i++) {
        crew.members[i].id = i;
        crew.members[i].state = IDLE;
        crew.members[i].current_order = NULL;
    }
}

void cleanup_crew(void) {
    pthread_mutex_destroy(&crew.crew_mutex);
}

void* crew_thread(void* arg) {
    CrewMember* member = (CrewMember*)arg;
    
    while (game_state.is_running) {
        if (member->current_order != NULL) {
            if (assign_workstation(member->id)) {
                member->state = PREPARING;
                update_display();
                sleep(DISHES[member->current_order->dish].prep_time);
                release_workstation(member->id);
                
                if (assign_kitchen(member->id)) {
                    member->state = COOKING;
                    update_display();
                    sleep(DISHES[member->current_order->dish].cook_time);
                    release_kitchen(member->id);
                    
                    remove_order(member->current_order->order_id);
                    member->current_order = NULL;
                }
            }
            
            member->state = IDLE;
            update_display();
        }
        
        usleep(100000); 
    }
    
    return NULL;
}

bool assign_order_to_crew(int crew_id, OrderNode* order) {
    bool success = false;
    pthread_mutex_lock(&crew.crew_mutex);
    
    if (crew_id >= 0 && crew_id < MAX_CREW && 
        crew.members[crew_id].state == IDLE && 
        crew.members[crew_id].current_order == NULL) {
        
        crew.members[crew_id].current_order = order;
        order->is_assigned = true;
        success = true;
    }
    
    pthread_mutex_unlock(&crew.crew_mutex);
    return success;
}
