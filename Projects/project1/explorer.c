#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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
    printf("It's time to see the world/file system!\n");
    // seed the srand
    srand(seed);

    for (int i = 0; i < 5; i++)
    {
        // gets option number to choose directory randomly
        int choice = rand() % 6; // gets choice number from
        char directory[10];
        //printf("choice: %d\n", choice);
        switch (choice)
        {
        case 0:
            strcpy(directory, "/home");
            break;
        case 1:
            strcpy(directory, "/proc");
            break;
        case 2:
            strcpy(directory, "/proc/sys");
            break;
        case 3:
            strcpy(directory, "/usr");
            break;
        case 4:
            strcpy(directory, "/usr/bin");
            break;
        case 5:
            strcpy(directory, "/bin");
            break;
        default:
            printf("This shouldn't print\n");
            break;
        }
        // print out chosen directory.
        //printf("%s\n", directory);

        // Change directory
        chdir(directory);

        // store the current working directory
        char currentDir[100];
        getcwd(currentDir, sizeof(currentDir));
        
        printf("Selection #%d: %s [SUCCESS]\n", i + 1, directory);
        // printf("Selection #%d: %s [SUCCESS]\n", i + 1, currentDir);

        // printf("Current reported directory %s\n", currentDir);
        printf("Current reported directory %s\n", directory);

        int rc = fork();

        if (rc < 0)
        { // fork failed
            fprintf(stderr, "fork failed \n");
            exit(1);
        }
        else if (rc == 0)
        { // we are in the new child process.
            int childPid = getpid();

            printf("  [Child, PID: %d] : Executing 'ls -tr' command...\n", childPid);

            // execute the command
            execvp("ls", (char *[]){"ls", "-tr", (char *)NULL});

            printf("The command failed :(");
            exit(0);
        }
        else
        { // We are in the parent process
            int status;
            printf("[Parent]: I am waiting for PID %d to finish \n", rc);
            waitpid(rc, &status, 0); // wait for child process 'rc' to return
            printf("[Parent]: Child %d finished with status code %d. Onward! \n", rc, WEXITSTATUS(status));
        }
    }
    return 0;
}