/**
 * Name: Shuangchang Li, Qiuan Wu
 * Assignment 1
 * 
 * I pledge my honor that I have abided by the Stevens Honor System.
 * 
**/

#include "header.hh"
void *producer (void *args) {

    using namespace std;

    int ID;
    ID = *((int *) args);

    while (global_var.num_product <= global_var.max_product) {

        pthread_mutex_lock(&global_var.lock_q);
        
        while (products.size() == global_var.q_size && global_var.q_size > 0 && global_var.num_product < global_var.max_product) {
            pthread_cond_wait(&global_var.q_notFull, &global_var.lock_q);
        }

        if (global_var.num_product <= global_var.max_product) {
            struct timespec tmp;
            clock_gettime(CLOCK_MONOTONIC, &tmp);
            t_product *prod = (t_product *) malloc(sizeof(t_product));
            prod->tstamp = tmp.tv_sec + (tmp.tv_nsec / 1000000000.0);
            prod->id = random() % 1024;
            prod->life = random() % 1024;
            products.push(prod);
            global_var.num_product++;
            cout << "Producer " << ID << " has produced product " << products.back()->id << endl;
        }

        fflush(stdout);
        pthread_mutex_unlock(&global_var.lock_q);
        pthread_cond_broadcast(&global_var.q_notEmpty);
        usleep(100000);
    }


    // Set the top time when the last thread exits
    clock_gettime(CLOCK_MONOTONIC, &global_var.p_Stoptime);

    return NULL;

}