/**
 * Name: Shuangchang Li, Qiuan Wu
 * Assignment 1
 * 
 * I pledge my honor that I have abided by the Stevens Honor System.
 * 
**/
#include "header.hh"

//Calculate fibnacci number

unsigned int fibo(unsigned int num)
{
    if (num == 0)
    {
        return 0;
    }
    else if (num == 1)
    {
        return 1;
    }
    else
    {
        return fibo(num - 1) + fibo(num - 2);
    }
}

void *consumer (void *args) {

    using namespace std;

    int ID;
    ID = *((int *) args);
    int i;
    float currT;

    do{
        pthread_mutex_lock(&global_var.lock_q);

        while (products.size() == 0 && global_var.p_consumed < global_var.max_product) {
            pthread_cond_wait(&global_var.q_notEmpty, &global_var.lock_q);
        }

        clock_gettime(CLOCK_MONOTONIC, &global_var.current_time);
        currT = global_var.current_time.tv_sec + (global_var.current_time.tv_nsec / 1000000000.0);

        // Scheduling algorithms
        if (global_var.schedule == 0) { 
            // First-Come-First-Serve
            t_product* currP = products.front();
            if (currP != NULL) {
                
                global_var.total_waiting = currT - currP->tstamp;

                if (currT - currP->tstamp > global_var.max_waiting) {
                    global_var.max_waiting = currT - currP->tstamp;
                }
                if (currT - currP->tstamp < global_var.min_waiting) {
                    global_var.min_waiting = currT - currP->tstamp;
                }
                for (i = 0; i < currP->life; i++) {
                    fibo(10);
                }
                cout << "Consumer " << ID  << " has consumed product " << currP->id << endl;
                products.pop();
                global_var.p_consumed++;

                if (currT - currP->tstamp > global_var.c_maxTime) {
                    global_var.c_maxTime = currT - currP->tstamp;
                }
                if (currT - currP->tstamp < global_var.c_minTime) {
                    global_var.c_minTime = currT - currP->tstamp;
                }

                global_var.turnSum += currT - currP->tstamp;
            }    
        }
        else {
            // Round-Robin
            t_product* currP = products.front();
            if (currP != NULL) {
                if (currT - currP->tstamp > global_var.max_waiting) {
                    global_var.max_waiting = currT - currP->tstamp;
                }
                if (currT - currP->tstamp < global_var.min_waiting) {
                    global_var.min_waiting = currT - currP->tstamp;
                }
                products.pop();

                // If product life greater than time sagment allowed
                if (currP->life >= global_var.quantum) {
                    currP->life = currP->life - global_var.quantum;

                    for (i = 0; i < global_var.quantum; i++) {
                        fibo(10);
                    }
                    products.push(currP);
                }
                else {
                    global_var.total_waiting += currT - currP->tstamp;
                    for (i = 0; i < currP->life; i++) {
                        fibo(10);
                    }
                    cout << "Consumer " << ID  << " has consumed product " << currP->id << endl;
                    global_var.p_consumed++;
                    if (currT - currP->tstamp < global_var.c_minTime) {
                        global_var.c_minTime = currT - currP->tstamp;
                    }
                    if (currT - currP->tstamp > global_var.c_maxTime) {
                        global_var.c_maxTime = currT - currP->tstamp;
                    }
                    global_var.turnSum += currT - currP->tstamp;
                    
                    
                }
            }
        }

        fflush(stdout);
        pthread_mutex_unlock(&global_var.lock_q);
        pthread_cond_broadcast(&global_var.q_notFull);
        usleep(100000);
        clock_gettime(CLOCK_MONOTONIC, &global_var.c_Stoptime);
    } while (global_var.p_consumed < global_var.max_product);
    // Set the top time when the last thread exits

    clock_gettime(CLOCK_MONOTONIC, &global_var.c_Stoptime);
    return NULL;

}
