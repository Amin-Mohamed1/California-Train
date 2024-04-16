#include <pthread.h>
#include "caltrain.h"
/*
 * Initialize the train station.
 * Parameters:
 * - station: Pointer to the station structure to be initialized.
 * Description:
 * - This function initializes the station by setting initial values for 
 *   available seats, passengers waiting, and passengers boarded.
 * - It also initializes mutex and condition variables used for synchronization.
 */
void station_init(struct station *station) {
    station->available_seats = 0;
    station->passengers_waiting = 0;
    station->passengers_boarded = 0;
   
    pthread_mutex_init(&(station->mutex), NULL);
    pthread_cond_init(&(station->train_arrival), NULL);
    pthread_cond_init(&(station->train_departure), NULL);
}

/*
 * Load the train with passengers.
 * Parameters:
 * - station: Pointer to the station structure.
 * - count: Number of available seats on the train.
 * Description:
 * - This function is called when a train arrives at the station.
 * - It updates the available seats on the train, broadcasts the train arrival
 *   to waiting passengers, and waits for passengers to board the train.
 */

void station_load_train(struct station *station, int count) {   
    pthread_mutex_lock(&(station->mutex)); 
    
    // If there are no available seats or no passengers waiting, unlock and return.
    if (!count || !station->passengers_waiting) {
        pthread_mutex_unlock(&(station->mutex));
        return; 
        // train must leave the station promptly
    }
    
    station->available_seats = count;
    // Signal passengers that train has arrived.
    pthread_cond_broadcast(&(station->train_arrival));

    // Wait for passengers to board.
    pthread_cond_wait(&(station->train_departure), &(station->mutex)); 

    // Reset available seats after boarding.
    station->available_seats = 0;

    pthread_mutex_unlock(&(station->mutex));
}

/*
 * Wait for the train to arrive and board the train.
 * Parameters:
 * - station: Pointer to the station structure.
 * Description:
 * - This function is called by passengers waiting for the train.
 * - It increments the count of passengers waiting, waits until there are 
 *   available seats on the train, and then boards the train.
 */

void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&(station->mutex));

    station->passengers_waiting++;

    // Wait for train arrival
    while (!station->available_seats)
        pthread_cond_wait(&(station->train_arrival), &(station->mutex));

    station->passengers_waiting--;
    // Increment boarded passengers count
    station->passengers_boarded++;

    // Decrease available seats.
    station->available_seats--;
    
    pthread_mutex_unlock(&(station->mutex));
}


/*
 * Signal the train departure if all passengers have boarded or no more passengers are waiting.
 * Parameters:
 * - station: Pointer to the station structure.
 * Description:
 * - This function is called when passengers have boarded the train.
 * - It checks if all passengers have boarded and no more passengers are waiting,
 *   then it signals the train departure.
 */
void station_on_board(struct station *station) {
    pthread_mutex_lock(&(station->mutex));
    
    // Decrement boarded passengers count.
    station->passengers_boarded--;

    // Signal train departure if conditions are met.
    if (!station->passengers_boarded && !(station->passengers_waiting && station->available_seats))
        pthread_cond_signal(&(station->train_departure));
    
    pthread_mutex_unlock(&(station->mutex));
}

// Done :)
