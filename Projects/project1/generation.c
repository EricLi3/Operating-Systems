#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int generateChildren(int lifespan){
    // Exit when the count becomes less than or equal to 0.
    if(lifespan <= 0){
        exit(0);
    }

    int rc = fork(); // Fork

    if (rc < 0){ // fork failed
        fprintf(stderr, "fork failed \n");
        exit(1);
    }
    else if (rc == 0){ // we are in the new child process.
        int childPid = getpid();
        if(lifespan > 0){

            lifespan--; // decrement the counter
            printf("   [Child, PID: %d] : I was called with descendant count=%d. I'll have %d descendant(s)\n", childPid, lifespan+1, lifespan);
            generateChildren(lifespan); // call the function recursively
        }
        else{
            exit(0);
        }
    }
    else{ // We are in the parent process
        int status;
        printf("[Parent, PID: %d]: I am waiting for PID %d to finish \n", getpid(), rc);
        waitpid(rc, &status, 0); // wait for child process 'rc' to return

        printf("[Parent, PID: %d]: Child %d finished with status code %d. It's now my turn to exit.\n", getpid(), rc, WEXITSTATUS(status));

        //exit with value 1 greater than child's exit value
        int exitCode = WEXITSTATUS(status) + 1;
        exit(exitCode);
    }
}

int main()
{
    // file which contains the seed
    FILE *seedFile = fopen("seed.txt", "r");
    if (seedFile == NULL)
    {
        perror("Error opening seed.txt");
        exit(EXIT_FAILURE);
    }

    char seedString[20];
    fscanf(seedFile, " %s", seedString);
    fclose(seedFile);
    // convert string to integer
    int seed = atoi(seedString);
    printf("Read seed value: %s\n\n", seedString);
    printf("Read seed value (converted to integer): %d\n", seed);

    // seed the srand
    srand(seed);

    // get random lifespan (between 5 and 12 inclusive)
    int numChildren = rand() % 8 + 5; // gets random number between 0 and 7 from the mod 8, and then adds 5 to get between 5 and 12.
    printf("Random Descendant Count: %d\n", numChildren);
    printf("Time to meet the kids/grandkids/great grandkids/...\n");

    generateChildren(numChildren);
    return 0;
}