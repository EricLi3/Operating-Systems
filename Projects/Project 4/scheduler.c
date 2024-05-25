#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Struct for a job
struct job {
    int id;
    int length;
    struct job *next;
};

// Struct for storing per-job analysis
struct job_analysis {
    int id;
    int response_time;
    int turnaround_time;
    int wait_time;
};

/*The scheduler takes three command line arguments: policy name, job trace, and timeslice.*/

// Function Prototypes
void fifo(struct job *jobs);
void sjf(struct job *jobs);
void round_robin(struct job *jobs, int time_slice);

void swap(struct job *a, struct job *b);

int main(int argc, char **argv){
    if(argc > 4 || argc < 4){
        printf("ERROR: input is formatted incorrectly. Should be in the form of 'policy name, job trace, and timeslice. \n");
    }
    // Get the input argunments into vars so we can use them. 
    char *policy_name = argv[1];
    char *workload = argv[2];
    int time_slice = atoi(argv[3]);

    // Open the workload file
    FILE *file = fopen(workload, "r");
    if(file == NULL){
        perror("Error opening file!");
        return 1;
    }

    // Initialize job list
    struct job *prev = NULL;
    struct job *head = NULL;
    int id = 0;
    int length;
    //int count = 0;

    while (fscanf(file, "%d", &length) == 1) {  // create the job list
        struct job *new_job = malloc(sizeof(struct job));
        if (new_job == NULL) {
            perror("Memory allocation failed");
            return 1;
        }
        new_job->id = id++;
        new_job->length = length;
        new_job->next = NULL;
       // count++;
        if (head == NULL) {
            head = new_job; // Initalize the head of the list
        } else {
            prev->next = new_job;
        }
        prev = new_job;
    }
    fclose(file);

    // Run scheduler based on policy
    if (strcmp(policy_name, "FIFO") == 0) {
        fifo(head);
    } else if (strcmp(policy_name, "SJF") == 0) {
        sjf(head);
    } else if (strcmp(policy_name, "RR") == 0) {
        round_robin(head, time_slice);
    } else {
        printf("Invalid policy\n");
        return 1;
    }

    return 0;
}

// FIFO policy implementation
void fifo(struct job *jobs) {
    printf("Execution trace with FIFO:\n");
    struct job *current = jobs;
    int total_turnaround = 0;
    int total_response = 0;
    int total_wait = 0;
    int time = 0;
    int num_jobs = 0;

    // execution trace
    while (current != NULL) {
        printf("Job %d ran for: %d\n", current->id, current->length);
        current = current->next;
    }
    printf("End of execution with FIFO.\n");

    // Print average performance
    printf("Begin analyzing FIFO:\n");

    current = jobs;
    while (current != NULL) {

        // Calculate metrics for the current job
        int response_time = time;
        int turnaround_time = time + current->length;
        int wait_time = time;

        // Update total metrics
        total_response += response_time;
        total_turnaround += turnaround_time;
        total_wait += wait_time;
        num_jobs++;

        // Move time forward
        time += current->length;

        // Print metrics for the current job
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
               current->id, response_time, turnaround_time, wait_time);

        current = current->next;
    }
    // Calculate average metrics
    float avg_response = (float)total_response / num_jobs;
    float avg_turnaround = (float)total_turnaround / num_jobs;
    float avg_wait = (float)total_wait / num_jobs;
    
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", 
           avg_response, avg_turnaround, avg_wait);
    printf("End analyzing FIFO.\n");
}

// SJF policy implementation
void sjf(struct job *jobs) {
    printf("Execution trace with SJF:\n");

    // Count the number of jobs
    int count = 0;
    struct job *temp = jobs;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    // Convert linked list to array for sorting
    struct job *job_array[count];

    temp = jobs;
    for (int i = 0; i < count; i++) {
        job_array[i] = temp;
        temp = temp->next;
    }

    // Bubble sort the job array by length ASCENDING
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (job_array[j]->length > job_array[j + 1]->length) {
                swap(job_array[j], job_array[j + 1]);
            }
        }
    }

    // Run the jobs in sorted order
    int time = 0;
    int total_turnaround = 0;
    int total_response = 0;
    int total_wait = 0;
    int num_jobs = 0;

    for (int i = 0; i < count; i++) {
        struct job *current_job = job_array[i];
        // Execute the job
        printf("Job %d ran for: %d\n", current_job->id, current_job->length);
        // Increment number of jobs
        num_jobs++;
    }

    printf("End of execution with SJF.\n");
    printf("Begin analyzing SJF:\n");

    // Print per-job analysis
    for (int i = 0; i < count; i++) {
        struct job *current_job = job_array[i];
        int response_time = time;
        int wait_time = response_time; // SJF, so wait time equals response time
        total_response += response_time;
        total_wait += wait_time;
        // Update time
        time += current_job->length;
        // Calculate turnaround time
        int turnaround_time = time;
        // Update total turnaround time
        total_turnaround += turnaround_time;
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               job_array[i]->id, response_time, turnaround_time, wait_time);
    }

    // Calculate average metrics
    float avg_response = (float)total_response / num_jobs;
    float avg_turnaround = (float)total_turnaround / num_jobs;
    float avg_wait = (float)total_wait / num_jobs;

    // Print average metrics
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n",
           avg_response, avg_turnaround, avg_wait);
    printf("End analyzing SJF.\n");
}


// Function to swap two jobs
void swap(struct job *a, struct job *b) {
    struct job temp = *a;
    *a = *b;
    *b = temp;
}

void round_robin(struct job *jobs, int time_slice) {
    printf("Execution trace with RR:\n");

    struct job *current = jobs;
    int total_turnaround = 0;
    int total_response = 0;
    int total_wait = 0;
    int num_jobs = 0;

    // Calculate the total number of jobs
    while (current != NULL) {
        num_jobs++;
        current = current->next;
    }

    // Reset current pointer
    current = jobs;

    // Initialize an array to track remaining time for each job
    int *remaining_time = (int *)malloc(num_jobs * sizeof(int));
    int *arrival_times = (int *)malloc(num_jobs * sizeof(int));
    for (int i = 0; i < num_jobs; i++) {
        remaining_time[i] = current->length;
        arrival_times[i] = 0; // Initialize arrival times to 0
        current = current->next;
    }

    int time = 0;
    int completed_jobs = 0;
    // Array of structs holding the per-job analysis
    struct job_analysis analysis[num_jobs];

    // Execution loop
    if (num_jobs == 1) {
        // If there's only one job, execute it until completion
        current = jobs;
        while (remaining_time[current->id] > 0) {
            int execute_time = (remaining_time[current->id] > time_slice) ? time_slice : remaining_time[current->id];
            printf("Job %d ran for: %d\n", current->id, execute_time);
            remaining_time[current->id] -= execute_time;
            time += execute_time;

            // Check if the job is completed
            if (remaining_time[current->id] == 0) {
                int response_time = arrival_times[current->id]; // Corrected calculation for response time
                int turnaround_time = time;
                int wait_time =  time - current->length;;

                // Store per-job analysis data in the array
                analysis[current->id].id = current->id;
                analysis[current->id].response_time = response_time;
                analysis[current->id].turnaround_time = turnaround_time;
                analysis[current->id].wait_time = wait_time;

                // Update total metrics
                total_response += response_time;
                total_turnaround += turnaround_time;
                total_wait += wait_time;
                completed_jobs++;
            }
        }
    } else {
        // If there are multiple jobs, follow the regular execution loop
        while (completed_jobs < num_jobs) {
            current = jobs; // Reset current pointer to the beginning of the job list

            // Process each job in the list
            while (current != NULL) {
                if (remaining_time[current->id] > 0) {
                    if (arrival_times[current->id] == 0) {
                        // Store arrival time
                        arrival_times[current->id] = time;
                    }
                    // Execute the job for the time slice or remaining time, whichever is smaller
                    int execute_time = (remaining_time[current->id] > time_slice) ? time_slice : remaining_time[current->id];

                    // Print execution trace
                    printf("Job %d ran for: %d\n", current->id, execute_time);

                    // Update remaining time
                    remaining_time[current->id] -= execute_time;
                    time += execute_time;

                    // Check if the job is completed
                    if (remaining_time[current->id] == 0) {
                        int response_time = arrival_times[current->id]; // Corrected calculation for response time
                        int turnaround_time = time;
                        // Calculate wait time
                        int wait_time =  time - current->length;;

                        // Store per-job analysis data in the array
                        analysis[current->id].id = current->id;
                        analysis[current->id].response_time = response_time;
                        analysis[current->id].turnaround_time = turnaround_time;
                        analysis[current->id].wait_time = wait_time;
                        
                        // Update total metrics
                        total_response += response_time;
                        total_turnaround += turnaround_time;
                        total_wait += wait_time;
                        completed_jobs++;
                    }
                }
                current = current->next;
            }
        }
    }

    printf("End of execution with RR.\n");
    printf("Begin analyzing RR:\n");

    // Free allocated memory
    free(remaining_time);
    free(arrival_times);

    // Print per-job analysis
    for (int i = 0; i < num_jobs; i++) {
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", 
               analysis[i].id, analysis[i].response_time, analysis[i].turnaround_time, analysis[i].wait_time);
    }
    
    // Calculate average metrics
    float avg_response = (float)total_response / num_jobs;
    float avg_turnaround = (float)total_turnaround / num_jobs;
    float avg_wait = (float)total_wait / num_jobs;

    // Print average metrics
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", avg_response, avg_turnaround, avg_wait);
    printf("End analyzing RR.\n");
}