#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h> 
#include "child.h"

pid_t spawn_child(sem_t* sem, sem_t* sem_print, char* shared_memory, char* child_name, int timestamp) 
{
    // Fork a new process
    pid_t pid = fork();

    // Copy the "SPAWN" message + start timestamp into shared memory to signal the child to spawn
    snprintf(shared_memory, 1024, "SPAWN %d", timestamp);

    // Check if the fork operation failed
    if (pid == -1) 
    {
        perror("Fork failed");
        exit(1);
    }

    // Child process code (pid == 0)
    if (pid == 0) 
    {
        // Call the child_process function
        child_process(sem, sem_print, shared_memory, child_name);
        exit(0); 
    }

    // Parent process code (pid > 0)
    printf("Spawned child %s\n", child_name);

    // Return the PID of the child process to the parent
    return pid;
}

void terminate_child(sem_t* sem, pid_t pid, char* shared_memory, char* child_name, int timestamp) 
{
    // Copy the "TERMINATE" message into shared memory to signal the child to terminate
    snprintf(shared_memory, 1024, "TERMINATE %d", timestamp);

    // Notify the child process to terminate by posting to the semaphore
    sem_post(sem); 

    // Wait for the child process to terminate
    waitpid(pid, NULL, 0);
}

void child_process(sem_t* sem, sem_t* sem_print, char* shared_memory, char* child_name) 
{
    // Initialize a counter to track the number of messages received
    int counter = 0;
    // Write down the start timestamp of the child process
    int start_timestamp = 0;
    // Write down the end timestamp of the child process
    int end_timestamp = 0;

    char command[LINE];

    // A SPAWN message + start timestamp is on shared memory segment  
    sscanf(shared_memory, "%s %d", command, &start_timestamp);

    while (1) 
    {
        // Wait for a message from the parent process by waiting on the semaphore
        sem_wait(sem); 

        // Check if the termination signal has been received from shared memory
        if (sscanf(shared_memory, "%s %d", command, &end_timestamp) == 2 && strcmp(command, "TERMINATE") == 0)
        {
            printf("Child %s received termination signal\n", child_name);
            break;
        }

        // Print the received message from the parent
        printf("Child %s received message: %s\n", child_name, shared_memory);

        // Increment the counter to keep track of the number of messages received
        counter++;

        sem_post(sem_print);
    }

    // Calculate the timestamps for the child process to run
    int time = end_timestamp - start_timestamp;
    printf("Child %s has runned for %d timestamps and has served %d requests!\n", child_name, time, counter);

    // Exit the child process
    exit(0);
}