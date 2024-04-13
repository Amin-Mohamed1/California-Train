#include <pthread.h>


struct station {
    int available_seats;      // Number of available seats in the train
    int passengers_waiting;   // Number of passengers waiting at the station
    int passengers_boarded;   // Number of passengers who have boarded the train
    
    pthread_mutex_t mutex;    // Mutex for thread synchronization
    pthread_cond_t train_arrival;    // Condition variable indicating the arrival of the train
    pthread_cond_t train_departure;  // Condition variable indicating the departure of the train
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
