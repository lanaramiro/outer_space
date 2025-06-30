#include "../inc/game.h"
#include "../inc/orders.h"
#include "../inc/kitchen.h"
#include "../inc/crew.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int main() {
    srand(time(NULL));
    
    init_game();
    show_intro();
    
    pthread_t timer_thread, order_gen_thread;
    pthread_create(&timer_thread, NULL, game_timer_thread, NULL);
    pthread_create(&order_gen_thread, NULL, order_generator_thread, NULL);
    
    for (int i = 0; i < MAX_CREW; i++) {
        pthread_create(&crew.members[i].thread, NULL, crew_thread, &crew.members[i]);
    }
    
    while (game_state.is_running) {
        update_display();
        
        int ch = wgetch(main_win);
        if (ch != ERR) {
            static int first_key = 0;
            if (first_key == 0) {
                if (ch >= '1' && ch <= '0' + MAX_CREW) {
                    first_key = ch;
                }
            } else {
                int crew_id = first_key - '1';
                DishType dish;
                
                switch (ch) {
                    case 'b': dish = BANANA_SPLIT; break;
                    case 'c': dish = COFEE; break;
                    case 'f': dish = FRIED_CHICKEN; break;
                    case 'r': dish = RAMEN; break;
                    default: first_key = 0; continue;
                }
                
                pthread_mutex_lock(&order_list.orders_mutex);
                OrderNode* current = order_list.head;
                while (current != NULL) {
                    if (!current->is_assigned && current->dish == dish) {
                        if (assign_order_to_crew(crew_id, current)) {
                            break;
                        }
                    }
                    current = current->next;
                }
                pthread_mutex_unlock(&order_list.orders_mutex);
                
                first_key = 0;
            }
        }
        
        usleep(50000); 
    }
    
    pthread_join(timer_thread, NULL);
    pthread_join(order_gen_thread, NULL);
    for (int i = 0; i < MAX_CREW; i++) {
        pthread_join(crew.members[i].thread, NULL);
    }
    
    cleanup_game();
    return 0;
}
