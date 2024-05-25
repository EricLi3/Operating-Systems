#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int val = atoi(argv[1]);
    int seed = __INT_MAX__;
    char seedString[20];

    switch (val)
    {
    case 1:
    {
        FILE *seedFile1 = fopen("seed_slug_1.txt", "r");
        if (seedFile1 == NULL)
        {
            perror("Error opening seed file for case 1");
            return 1;
        }
        fscanf(seedFile1, " %s", seedString);
        fclose(seedFile1);
        seed = atoi(seedString);
        printf("[Slug PID: %d] Read seed value: %s\n\n", getpid(), seedString);
        printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);
        break;
    }

    case 2:
    {
        FILE *seedFile2 = fopen("seed_slug_2.txt", "r");
        if (seedFile2 == NULL)
        {
            perror("Error opening seed file for case 2");
            return 1;
        }
        fscanf(seedFile2, " %s", seedString);
        fclose(seedFile2);
        seed = atoi(seedString);
        printf("[Slug PID: %d] Read seed value: %s\n\n", getpid(), seedString);
        printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);
        break;
    }
    case 3:
    {
        FILE *seedFile3 = fopen("seed_slug_3.txt", "r");
        if (seedFile3 == NULL)
        {
            perror("Error opening seed file for case 3");
            return 1;
        }
        fscanf(seedFile3, " %s", seedString);
        fclose(seedFile3);
        seed = atoi(seedString);
        printf("[Slug PID: %d] Read seed value: %s\n\n", getpid(), seedString);
        printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);
        break;
    }

    case 4:
    {
        FILE *seedFile4 = fopen("seed_slug_4.txt", "r");
        if (seedFile4 == NULL)
        {
            perror("Error opening seed file for case 4");
            return 1;
        }
        fscanf(seedFile4, " %s", seedString);
        fclose(seedFile4);
        seed = atoi(seedString);
        printf("[Slug PID: %d] Read seed value: %s\n\n", getpid(), seedString);
        printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);
        break;
    }
    default:
        fprintf(stderr, "Invalid value for 'val'\n");
        return 1;
    }

    srand(seed);

    // calculate the sleepTime
    int sleepTime = rand() % 5 + 2;
    // calculate the coinFilp
    int coinFlip = rand() % 2;
    
    printf("[Slug PID: %d] Delay time is %d seconds. Coin flip: %d\n", getpid(), sleepTime, coinFlip);
    printf("[Slug PID: %d] I'll get the job done. Eventually...\n", getpid());

    // sleep for the calculated time
    sleep(sleepTime);

    printf("[Slug PID: %d] Break time is over! I am running the 'id -group' command.\n", getpid());

    if (coinFlip == 1)
    {
        execvp("id", (char *[]){"id", "--group", (char *)NULL});
    }
    else
    {
        execvp("last", (char *[]){"last", "-i", "-x", (char *)NULL});
    }
    return 0;
}
