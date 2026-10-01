#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


// Values similar for both cases
#define MAX_RAND 99
#define NUM_ITERATIONS 10


// Number of different experiments and list with all experiments for small case of 120 numbers
#define ELEMENT_NUM 120
#define NUM_EXPERIMENTS 5
#define EXPERIMENT_LIST (int[]){1, 6, 20, 60, 120}
#define LOG_STRING "CASE WITH 120 ELEMENTS:\n"


// Number of different experiments and list with all experiments for large case of 1M numbers
// #define ELEMENT_NUM 120
// #define NUM_EXPERIMENTS 7
// #define EXPERIMENT_LIST (int[]){1, 10, 100, 1000, 10000, 100000, 1000000}
// #define LOG_STRING "CASE WITH 1M ELEMENTS:\n"

typedef struct {
    int * u_ptr;
    int * v_vtr;
} Record;
 
struct Record* create_arrays(void) {
    int *u = malloc(ELEMENT_NUM*sizeof(int));
    int *v = malloc(ELEMENT_NUM*sizeof(int));
    Record *r = malloc(sizeof(Record));

    if (u == NULL || v == NULL || r == NULL) {
        return NULL;
    }

    r->u_ptr = u;
    r->v_vtr = v;

    return r;
} 

void fill_array(int *p) {
    for (int i = 0; i < ELEMENT_NUM; ++i) {
        *(p + i) = rand() % RAND_MAX;
    }
}

void fill_struct(Record *r) {
    if (r != NULL && r->u_ptr != NULL && r->v_vtr != NULL) {
        fill_array(r->u_ptr);
        fill_array(r->v_vtr);
    }
}

void dot_product(int *u, int *v, int left, int right) {
    //calculate dot poduct of fragment and put info in file
    ;
}

void distribute_calculations(Record *r) {
    // distribute fragments between n processes and wait for them
    ;
}

void run_iteration() {
    // run n iterations to get average results. then time answer into file
    ;
}

void run_experiment() {
    // move through array of experiments to run each for n iiterations
}

int main() {
    // do something, maybe create files

    return 0;
}


