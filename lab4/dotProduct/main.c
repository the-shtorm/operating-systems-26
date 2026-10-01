#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>


// -- Small explanation of what i did --
// I used shared memory with mmap to avoid array copying
// between thousands of child processes 
// Also i used pwrite (at least tried) to avoid races in file
// writing between multiple processes, because i think 10 000
// processes will definetly somehow interfere during file writing
// Additionally i executed each experiment several times to get 
// somewhat more stable results
// Also i used `short` data type for arrays to reduce memory load 
// (i'm not sure is it really useful here)

// Values similar for both cases
#define MAX_RAND 99
#define NUM_ITERATIONS 10
#define RESULTS_FILE "partial_results"


// Number of different experiments and list with all experiments for small case of 120 numbers
// #define NUM_ELEMENTS 120
// #define NUM_EXPERIMENTS 5
// #define LOG_STRING "CASE WITH 120 ELEMENTS:\n"
// static const int EXPERIMENT_LIST[NUM_EXPERIMENTS] = {1, 6, 20, 60, 120};


// Number of different experiments and list with all experiments for large case of 1M numbers
#define NUM_ELEMENTS 120
#define NUM_EXPERIMENTS 7
#define LOG_STRING "CASE WITH 1M ELEMENTS:\n"
static const int EXPERIMENT_LIST[NUM_EXPERIMENTS] = {1, 10, 100, 1000, 10000, 100000, 1000000};


typedef struct {
    short * u_ptr;
    short * v_ptr;
    size_t total_size;
} Record;
 
Record* create_arrays(void) {
    size_t struct_size = sizeof(Record);
    size_t array_size = NUM_ELEMENTS * sizeof(short);
    size_t total_size = struct_size + (2 * array_size);

    void *raw = mmap(NULL, total_size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS,
                     -1, 0);

    if (raw == MAP_FAILED) {
        printf("Allocation failed\n");
        return NULL;
    }

    Record *r = (Record *)raw;
    r->total_size = total_size;

    r->u_ptr = (short *)((char *)raw + struct_size);
    r->v_ptr = (short *)((char *)r->u_ptr + array_size);

    return r;
} 

void free_arrays(Record *r) {
    if (r != NULL) {
        munmap(r, r->total_size);
    }
}

void fill_array(short *p) {
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        *(p + i) = (short)(rand() % RAND_MAX);
    }
}

void fill_struct(Record *r) {
    if (r != NULL && r->u_ptr != NULL && r->v_ptr != NULL) {
        fill_array(r->u_ptr);
        fill_array(r->v_ptr);
    }
}

void dot_product(short *u, short *v, int left, int right, 
                    int descriptor, __pid_t pid) {
    long long partial_sum;  // maximum sum for 1M array will be slightly out of integer limit

    for (int i = left; i < right; i++) {
        partial_sum += (long long)(u[i] * v[i]);
    }

    off_t offset = (off_t)(pid * sizeof(long long));

    if (pwrite(descriptor, (const void*)(&partial_sum), sizeof(long long), offset) == -1) {
        printf("Sum writing failed for child %d\n", pid);
    }
}

long long distribute_calculations(Record *r, int num_procs) {
    int descriptor = open(RESULTS_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (descriptor == -1) {
        printf("Open failed\n");
        return -1;
    }

    int chunk = (int)(NUM_ELEMENTS / num_procs);
    int left_index = 0;

    for (int i = 0; i < num_procs; ++i) {
        int right_index = left_index + chunk;

        __pid_t pid = fork();

        if (pid < 0) {
            printf("Fork failed");
            close(descriptor);
            return -1;
        }

        if (pid == 0) {
            dot_product(r->u_ptr, r->v_ptr, left_index, right_index, descriptor, i);
            close(descriptor);
            _exit(0);   // according to internet here should be used _exit(0) instead of exit(0) to shutdown child without additional actions from system
        }

        left_index = right_index;
    }

    for (int i = 0; i < num_procs; i++) wait(NULL); // i wait for exactly num_procs exits from child processes
        long long total_sum = 0;
        long long temp = 0;

        for (int i = 0; i < num_procs; ++i) {
        off_t offset = (off_t)(i * sizeof(long long));

        pread(descriptor, &temp, sizeof(long long), offset);
        total_sum += temp;
    }

    close(descriptor);
    return (long long)total_sum;
}

double run_iteration(Record *r, int num_procs) {
    double total_time = 0.0;

    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        clock_t time;

        time = clock();
        distribute_calculations(r, num_procs);
        time = clock() - time;

        double elapsed = (double)(time);
        total_time += elapsed;
    }

    return total_time / NUM_ITERATIONS;
}

void run_experiment(Record *r) {
    printf("%s", LOG_STRING);
    printf("Procs\t| Avg Time (mc sec)\n");
    printf("------------------------\n");

    for (int i = 0; i < NUM_EXPERIMENTS; ++i) {
        int procs = EXPERIMENT_LIST[i];
        double avg_time = run_iteration(r, procs);
        printf("%d\t| %.6f\n", procs, avg_time);
    }
}

int main() {
    srand(67);

    Record *r = create_arrays();
    if (r == NULL) {
        return 1;
    }

    fill_struct(r);
    run_experiment(r);

    free_arrays(r);
    unlink(RESULTS_FILE);

    return 0;
}


