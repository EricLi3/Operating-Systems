#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    //file which contains the seed
    FILE *seedFile = fopen("seed.txt", "r");
    if(seedFile == NULL){
        perror("Error opening seed.txt");
        exit(EXIT_FAILURE);
    }

    char seedString[20];
    fscanf(seedFile," %s", seedString);
    fclose(seedFile);
    //convert string to integer 
    int seed = atoi(seedString);
    printf("Read seed value: %s\n\n", seedString);
    printf("Read seed value (converted to integer): %d\n", seed);

    //seed the srand
    srand(seed);

    // get random number of children (between 8 and 13 inclusive)
    int numChildren = rand() % 6 + 8; // gets random number between 0 and 5 from the mod 6, and then adds 8 to get between 8 and 13. 
    printf("Random Child Count: %d\nI'm feeling prolific!\n", numChildren);
    
    // stores random numbers for each children
    int randomNumbers[numChildren];

    // populates the array with random numbers for each children.
    for(int i = 0; i < numChildren; i++){
        randomNumbers[i] = rand();    
    }

    for(int i = 0; i < numChildren; i++){
        int rc = fork();

        if(rc < 0){ // fork failed
            fprintf(stderr,"fork failed \n");
            exit(1);
        } 
        else if(rc == 0){ // we are in the new child process. 
            int childPid = getpid();
            int yourRandomNum = randomNumbers[i];

            //Calculate the exit code and wait time
            int exitCode = (yourRandomNum % 50) + 1;
            int waitTime = (yourRandomNum % 3) + 1;

            printf("  [Child, PID: %d] : I am the child and I will wait %d seconds and exit with code %d\n", childPid, waitTime, exitCode);

            sleep(waitTime);
            printf("  [Child, PID: %d] : Now exiting...\n", childPid);
            exit(exitCode);
        }
        else{ // We are in the parent process
            int status;
            printf("[Parent]: I am waiting for PID %d to finish \n", rc);
            waitpid(rc, &status, 0); // wait for child process 'rc' to return
            printf("[Parent]: Child %d finished with status code %d. Onward! \n", rc, WEXITSTATUS(status));
        }
    }
    return 0;
}