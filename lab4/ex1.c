#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int n = 4;
    __pid_t pid =  fork();

    if (pid < 0) {
        printf("Fork failure");

        return EXIT_FAILURE;
    }

    if (pid == 0) {
        sleep(1);
        __pid_t pid1 = fork();

        if (pid1 < 0) {
            printf("Fork failure");

            return EXIT_FAILURE;
        }
        
        if (pid1 == 0) {
            sleep(1);
            printf("Hello from child 2\n");
            printf("Beautiful output:\nPID: %d, Fork: %d\n\n", getpid(), pid);

            return 0;
        } else {
            printf("Hello from child 1\n");
            printf("Beautiful output:\nPID: %d, Fork: %d\n\n", getpid(), pid);
            return 0;
        }
    } else {
        printf("Hello from parent\n");
        printf("Beautiful output:\nPID: %d, Fork: %d\n\n", getpid(), pid);
    }

    return 0;
}