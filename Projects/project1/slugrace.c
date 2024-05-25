#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define BILLION 1000000000L

double calculate_time_taken(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double)BILLION;
}

void print_time_diff(struct timespec start_time)
{
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double time_diff = calculate_time_taken(start_time, end_time);

    printf("Time taken: %.6f seconds\n", time_diff);
}

int main()
{
    // Stores PID numbers of children/slugs
    pid_t child_pids[4];

    // record the start time of the race
    // struct timespec start_time;
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 1; i <= 4; i++)
    {
        int rc = fork();

        if (rc < 0)
        { // fork failed
            fprintf(stderr, "fork failed \n");
            exit(1);
        }
        else if (rc == 0)
        { // we are in the new child process.
            int childPid = getpid();
            printf("  [Child, PID: %d] : Executing './slug %d' command...\n", childPid, i);
            usleep(40000);

            // running specific slug program
            switch (i)
            {
            case 1:
                execvp("./slug", (char *[]){"slug", "1", (char *)NULL});
                break;
            case 2:
                execvp("./slug", (char *[]){"slug", "2", (char *)NULL});
                break;
            case 3:
                execvp("./slug", (char *[]){"slug", "3", (char *)NULL});
                break;
            case 4:
                execvp("./slug", (char *[]){"slug", "4", (char *)NULL});
                break;
            default:
                printf("Well, something went wrong...");
                break;
            }
            // Add this line to exit the child process after execvp
            exit(0);
        }
        else
        { // We are in the parent process
            child_pids[i - 1] = rc;
            printf("[Parent]: I forked off child %d\n", rc);
        }
        // Introduce a sleep to allow the next child to start
        usleep(10000);
    }

    int finishedChildren = 0;
    int activeProcesses[4];

    while (finishedChildren < 4)
    {
        int counter = 0;
        for (int i = 0; i < 4; i++)
        {
            int status;
            int result = waitpid(child_pids[i], &status, WNOHANG);

            if (result > 0)
            { // child i is finished
                if (activeProcesses[i] == 1)
                {
                    printf("Child %d has crossed the finish line!\n", child_pids[i]);
                    print_time_diff(start_time);

                    finishedChildren++;
                    activeProcesses[i] = 0;
                }
            }
            else if (result == 0)
            {
                counter++;
                // Child i is still active
                activeProcesses[i] = 1;
            }
        }
        if (counter != 0)
            printf("The race is ongoing. The following children are still racing: ");

        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (activeProcesses[i] == 1)
            {
                count++;
                printf("%d ", child_pids[i]);
            }
        }
        printf("\n");

        if (count == 0)
        {
            struct timespec end_time_whole;
            clock_gettime(CLOCK_MONOTONIC, &end_time_whole);
            double time = calculate_time_taken(start_time, end_time_whole);
            printf("The race is over! It took %.6f seconds\n", time);
        }
        // sleep for 0.33 seconds.
        usleep(330000);
    }

    return 0;
}
