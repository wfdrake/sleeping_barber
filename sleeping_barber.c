#include "ezipc.h"
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void barber();
void customer(int customerName);
void getHaircut();
void randomWait(int maxSeconds);
void handler(int signum);

char *openChairs;
char *numHaircuts;

int openChairsSem;
int barberSem;
int customerSem;
int inBarberSeatSem;
int customerPaysSem;

int main() {
        // Set up ezipc, signal handler, and rand()
        SETUP();
        signal(SIGUSR1, handler);
        srand(time(0));

        int maxNumCustomers;
        int numChairs;
        int barberPID = getpid();

        printf("Barber process pid is: %d\n", barberPID);

        // Get user input
        printf("Enter maximum number of customers: ");
        scanf("%d", &maxNumCustomers);
        printf("Enter number of chairs: ");
        scanf("%d", &numChairs);
        printf("Maximum number of customers: %d\n", maxNumCustomers);
        printf("Number of chairs: %d\n\n", numChairs);

        printf("To kill barber process: kill -SIGUSR1 %d \n\n", barberPID);

        // Set up shared memory, tracks # of open chairs and haircuts
        openChairs = SHARED_MEMORY(10);
        *openChairs = numChairs;
        numHaircuts = SHARED_MEMORY(10);
        *numHaircuts = 0;

        // Make semaphores
        openChairsSem = SEMAPHORE(0, numChairs);
        barberSem = SEMAPHORE(1, 0);
        customerSem = SEMAPHORE(1, 1);
        inBarberSeatSem = SEMAPHORE(1, 1);
        customerPaysSem = SEMAPHORE(1, 1);

        int pid1;
        int pid2;

        // Forks a child spawning process which then forks the customer processes
        // The barber process is the main process
        pid1 = fork();
        if (pid1 < 0) {
		// Fork failed
                printf("Fork failed\n");
		return 1;
        }
        else if (pid1 > 0) {
                barber();
        }
        else {
                for (int i = 1; i <= maxNumCustomers; i++) {
                        randomWait(4);
                        pid2 = fork();

                        if (pid2 < 0) {
                                // Fork failed
                		printf("Fork failed\n");
				return 1;
                        }
                        else if (pid2 == 0) {
                                customer(i);
                                break;
                        }
                }
        }
        return 0;
}

// Method for customer processes. customerName parameter is what # customer it is
void customer(int customerName) {
        P(customerSem);
        printf("Customer %d arrived, there are %d seats available\n", customerName, *openChairs);
        if ((*openChairs) == 0) {
                printf("OH NO! Customer %d leaves, no chairs available\n", customerName);
                V(customerSem);
        }
        else {
                P(openChairsSem);

                //openChairs--;
                int chairs1 = *openChairs;
                chairs1--;
                *openChairs = chairs1;

                V(customerSem);
                V(barberSem);
                P(inBarberSeatSem);
                V(openChairsSem);

                //openChairs++;
                int chairs2 = *openChairs;
                chairs2++;
                *openChairs = chairs2;

                getHaircut(customerName);
                V(inBarberSeatSem);
                printf("$$ Customer %d gets in line to pay cashier\n", customerName);
                P(customerPaysSem);
                randomWait(3);
                printf("$$$$$$$ Customer %d done paying and leaves, bye!\n", customerName);
                V(customerPaysSem);
        }
        return;
}

// Method for barber process
void barber() {
        while(1) {
                P(barberSem);
        }
        return;
}

// Method for describing a haircut. customerName parameter is what # customer it is
void getHaircut(int customerName) {
        printf("**** HAIR CUT! Customer %d is getting a haircut\n", customerName);
        randomWait(10);

        // haircuts++
        int haircuts = *numHaircuts;
        haircuts++;
        *numHaircuts = haircuts;

        printf("### Finished giving haircut to customer %d. That is haircut %d today\n", customerName, *numHaircuts);
        return;
}

// Method that sleeps for a random amount of seconds. Parameter is max amount of seconds
void randomWait(int maxSeconds) {
        sleep((rand() % maxSeconds) + 1);
        return;
}

//SIGUSR1 handler
void handler(int signum) {
        printf("Barber received USR1 smoke signal\n");
        printf("Total number of haircuts: %d\n", *numHaircuts);
        printf("Close shop and go home, bye bye!\n");
}
