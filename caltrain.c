#include <pthread.h>
#include "caltrain.h"

void station_init(struct station *station) {

    station->available_seats = 0;
    station->passengers_waiting = 0;
    station->passengers_boarded = 0;

    pthread_mutex_init(&(station->mutex), NULL);
    pthread_cond_init(&(station->train_arrival), NULL);
    pthread_cond_init(&(station->train_departure), NULL);
}

void station_load_train(struct station *station, int count) {   
    pthread_mutex_lock(&(station->mutex)); 
    
    // no available seats available or no passengers waiting
    if (!count || !station->passengers_waiting) {
        pthread_mutex_unlock(&(station->mutex));
        return; // train must leave the station promptly
    }

    station->available_seats = count;
    pthread_cond_broadcast(&(station->train_arrival)); // wake up all passengers waiting for a available seat
    pthread_cond_wait(&(station->train_departure), &(station->mutex)); // waiting for all passengers to get on board
    station->available_seats = 0;

    pthread_mutex_unlock(&(station->mutex));
}

void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&(station->mutex));

    station->passengers_waiting++;

    // Wait until there are available seats on the train
    while (!station->available_seats)
        pthread_cond_wait(&(station->train_arrival), &(station->mutex));

    station->passengers_waiting--;
    station->passengers_boarded++;
    station->available_seats--;
    
    pthread_mutex_unlock(&(station->mutex));
}

void station_on_board(struct station *station) {
    pthread_mutex_lock(&(station->mutex));
    
    station->passengers_boarded--;
    if (!station->passengers_boarded && !(station->passengers_waiting && station->available_seats)) // all leaving passengers are on board, and no other passengers can get on the train
        pthread_cond_signal(&(station->train_departure));
    
    pthread_mutex_unlock(&(station->mutex));
}

