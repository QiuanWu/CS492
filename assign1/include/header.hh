#ifndef _ASSIGN1_HH_
#define _ASSIGN1_HH_

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cfloat>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <queue>

// Product attributes
typedef struct s_product {
    int life;
    float tstamp;
    int id;
} t_product;

// Global variables
typedef struct s_global_variable {
    // Producer  
    pthread_cond_t q_notFull;
    int num_product;
    int max_product;
    struct timespec p_Stoptime;
    unsigned int q_size;

    // Consumer 
    pthread_cond_t q_notEmpty;
    int p_consumed;
    int quantum;
    struct timespec c_Starttime;
    struct timespec c_Stoptime;

    // Benchmark variables
    float c_minTime;
    float c_maxTime;
    float min_waiting;
    float max_waiting;
    float total_waiting;
    struct timespec start_time;
    struct timespec current_time;

    // Queue mutex
    pthread_mutex_t lock_q;

    // Scheduling algorithm 
    unsigned int schedule;

} t_global_variable;

// Export variables
extern t_global_variable global_var;
extern std::queue<t_product *> products;
unsigned int fibo(unsigned int num);
void createProduct();
void *producer(void *);
void *consumer(void *);

#endif
