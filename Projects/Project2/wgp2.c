#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_BASEBALL_PLAYERS 36
#define NUM_FOOTBALL_PLAYERS 44
#define NUM_RUGBY_PLAYERS 60

pthread_mutex_t baseballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t footballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rugbyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gameStartMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pairReadyMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t baseballOutputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t footballOutputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rugbyOutputMutex = PTHREAD_MUTEX_INITIALIZER;

int baseballPlayerCount = 0;
int footballPlayerCount = 0;
int rugbyPlayerCount = 0;
int totalRugbyPlayerCount = 0;

int footballSlotCount = 0;
int baseballSlotCount = 0;

typedef struct {
    int playerNumber;
    char sportType; // 'B' for baseball, 'F' for football, 'R' for rugby
} Player;

// Enumeration for game types
enum GameType {
    BASEBALL,
    FOOTBALL,
    RUGBY
};

// Variable to hold the current game type
enum GameType currentGameType = BASEBALL;

int isFootballGameStarted() {
    // Check if the football game has started
    return footballPlayerCount >= 22;
}

int isRugbyGameStarted() {
    // Check if the rugby game has started
    return rugbyPlayerCount >= 60;
}

void randomDelay() {
    int delay = rand() % 1000;
    usleep(delay * 1000); // Convert to microseconds
}

void initializeSeed() {
    FILE *seedFile = fopen("seed.txt", "r");
    if (seedFile == NULL) {
        fprintf(stderr, "Error opening seed.txt\n");
        exit(1);
    }

    unsigned int seed;
    fscanf(seedFile, "%u", &seed);
    fclose(seedFile);

    srand(seed);
}

void leaveField(char sportType, int fieldSlot) {
    if (sportType == 'B') {
        pthread_mutex_lock(&baseballOutputMutex);
    } else if (sportType == 'F') {
        pthread_mutex_lock(&footballOutputMutex);
    } else if (sportType == 'R') {
        pthread_mutex_lock(&rugbyOutputMutex);
    }

    printf("<<GAME ENDED>> %c game has ended. Players leaving the field from slot %d.\n", sportType, fieldSlot);

    if (sportType == 'B') {
        pthread_mutex_unlock(&baseballOutputMutex);
    } else if (sportType == 'F') {
        pthread_mutex_unlock(&footballOutputMutex);
    } else if (sportType == 'R') {
        pthread_mutex_unlock(&rugbyOutputMutex);
    }
}

void printGameStart(char sportType) {
    if (sportType == 'B') {
        pthread_mutex_lock(&baseballOutputMutex);
    } else if (sportType == 'F') {
        pthread_mutex_lock(&footballOutputMutex);
    } else if (sportType == 'R') {
        pthread_mutex_lock(&rugbyOutputMutex);
    }

    printf("<<GAME STARTED>> We have enough Players for %c game, so we are starting!\n", sportType);

    // Update the currentGameType variable inside the mutex-protected section
    currentGameType = (sportType == 'B') ? BASEBALL : (sportType == 'F') ? FOOTBALL : RUGBY;

    if (sportType == 'B') {
        pthread_mutex_unlock(&baseballOutputMutex);
    } else if (sportType == 'F') {
        pthread_mutex_unlock(&footballOutputMutex);
    } else if (sportType == 'R') {
        pthread_mutex_unlock(&rugbyOutputMutex);
    }
}


void printRugbyPairReady() {
    pthread_mutex_lock(&pairReadyMutex);
    printf("<<PAIR READY>> Pair of rugby players ready!\n");
    pthread_mutex_unlock(&pairReadyMutex);
}

static int currentRugbyPlayerNumber = 0;

void printBaseballMessage(int playerNumber, int currentSlot) {
    pthread_mutex_lock(&baseballOutputMutex);
    printf("Baseball Player %d joined at slot %d.\n", playerNumber, currentSlot);
    pthread_mutex_unlock(&baseballOutputMutex);
}

void printFootballMessage(int playerNumber, int currentSlot) {
    pthread_mutex_lock(&footballOutputMutex);
    printf("Football Player %d Joined at slot %d.\n", playerNumber, currentSlot);
    pthread_mutex_unlock(&footballOutputMutex);
}

void printRugbyMessage(int playerNumber, int currentSlot) {
    pthread_mutex_lock(&rugbyOutputMutex);
    printf("Rugby Player %d playing at slot %d.\n", playerNumber, currentSlot);
    pthread_mutex_unlock(&rugbyOutputMutex);
}

void *playerThread(void *arg) {
    Player *player = (Player *)arg;

    if (player->sportType == 'B') { // baseball case
        pthread_mutex_lock(&baseballMutex);

        int currentBaseballPlayerNumber = ++baseballPlayerCount;
        int currentBaseballSlot = (currentBaseballPlayerNumber - 1) % 18 + 1;

        printBaseballMessage(currentBaseballPlayerNumber, currentBaseballSlot);

        if (baseballPlayerCount % 18 == 0) {
            printGameStart('B'); // Print game start message

            // Simulate playing baseball
            randomDelay();
            leaveField('B', currentBaseballSlot);
        }

        pthread_mutex_unlock(&baseballMutex);
    } else if (player->sportType == 'F') { // football case
        pthread_mutex_lock(&footballMutex);

        int currentFootballPlayerNumber = ++footballPlayerCount;
        int currentFootballSlot = (currentFootballPlayerNumber - 1) % 22 + 1;

        printFootballMessage(currentFootballPlayerNumber, currentFootballSlot);
        footballSlotCount++;
        

        if (footballPlayerCount % 22 == 0) {
            printGameStart('F'); // Print game start message

            // Simulate playing football
            randomDelay();
            leaveField('F', currentFootballSlot);
        }

        pthread_mutex_unlock(&footballMutex);
    } else if (player->sportType == 'R') { // rugby case
        pthread_mutex_lock(&rugbyMutex);

        if (isFootballGameStarted() && !isRugbyGameStarted()) {
            // Check if the football game has started and the rugby game has not reached the limit
            ++currentRugbyPlayerNumber;
            int currentRugbySlot = (currentRugbyPlayerNumber - 1) % 30 + 1;

            printRugbyMessage(currentRugbyPlayerNumber, currentRugbySlot);

            if (currentRugbyPlayerNumber % 2 == 0) { // Pair of rugby players
                printRugbyPairReady(); // Print "pair ready" message
                totalRugbyPlayerCount++; // Increment the total rugby player count

                if (totalRugbyPlayerCount == 60) {
                    currentRugbyPlayerNumber = 0; // Reset the count for the next game
                    totalRugbyPlayerCount = 0; // Reset the total count for the next game
                    leaveField('R', currentRugbySlot);
                } else if (totalRugbyPlayerCount == 30) {
                    leaveField('R', currentRugbySlot);
                    currentRugbyPlayerNumber = 0; // Reset the count for the next game
                    printGameStart('R');
                    randomDelay();
                }
            }
        }
        pthread_mutex_unlock(&rugbyMutex);
    }
    return NULL;
}

int main(void) {
    initializeSeed();

    pthread_t tid[NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS];

    // Create threads for baseball, football, and rugby players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS; i++) {
        Player *player = malloc(sizeof(Player));
        player->playerNumber = i + 1;

        if (i < NUM_BASEBALL_PLAYERS) {
            player->sportType = 'B';
        } else if (i < NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS) {
            player->sportType = 'F';
        } else {
            player->sportType = 'R';
        }

        pthread_create(&tid[i], NULL, playerThread, player);
    }

    // Join threads for baseball, football, and rugby players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
