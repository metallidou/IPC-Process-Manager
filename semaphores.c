#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "semaphores.h"

sem_t** create_semaphores_array(int M)
{
    // Allocate memory for an array of M semaphores
    sem_t** semaphores = malloc(M * sizeof(sem_t*));

    for (int i = 0; i < M; i++) 
    {
        // Create semaphore name and store it in sem_name
        char sem_name[32];
        snprintf(sem_name, sizeof(sem_name), "/sem_%d", i);

        // Unlink any existing semaphore with sem_name
        sem_unlink(sem_name); 

        // Create and open the semaphore with the name sem_name
        semaphores[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 0);
        if (semaphores[i] == SEM_FAILED) 
        {
            // Handle failure to create or open the semaphore
            perror("Semaphore creation failed");
            exit(1);
        }
    }
    return semaphores;
}

sem_t* create_semaphore_print()
{
    sem_t* sem_print;

    // Create semaphore name and store it in sem_name
    char sem_name[32];
    snprintf(sem_name, sizeof(sem_name), "/sem_print");

    // Unlink any existing semaphore with sem_name
    sem_unlink(sem_name); 

    // Create and open the semaphore with the name sem_name
    sem_print = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 0);
    if (sem_print == SEM_FAILED) 
    {
        // Handle failure to create or open the semaphore
        perror("Semaphore creation failed");
        exit(1);
    }
    return sem_print;
}

void cleanup_semaphores_array(sem_t** semaphores, int M)
{
    for (int i = 0; i < M; i++) 
    {
        // Get the semaphore name for each semaphore in the array
        char sem_name[32];
        snprintf(sem_name, sizeof(sem_name), "/sem_%d", i);

        // Close the semaphore and unlink it to ensure proper cleanup
        sem_close(semaphores[i]);
        sem_unlink(sem_name);
    }
    free(semaphores);
}

void cleanup_semaphore_print(sem_t* sem_print)
{
    // Get the semaphore name for each semaphore in the array
    char sem_name[32];
    snprintf(sem_name, sizeof(sem_name), "/sem_print");

    // Close the semaphore and unlink it to ensure proper cleanup
    sem_close(sem_print);
    sem_unlink(sem_name);
}

int assign_semaphore_to_process(Processes process, int process_index, int M)
{
    // sem_indexes is an array that contains possible indexes for M semaphores
    // 0, 1, 2, ..., M-1
    int* sem_indexes = malloc(M * sizeof(int));
    for (int i = 0; i < M; i++)
        sem_indexes[i] = i;

    for (int i = 0; i < process.n; i++)
    {
        int sem_index = process.info[i].sem_index; 
        // If a sem_index is given to an active process, then exclude it from the available ones
        // ex. 0, 1, 2, ..., M-1 exclude 2 --> ex. 0, 1, -1, ..., M-1 
        if (sem_index != -1 && process.info[i].active)
            sem_indexes[sem_index] = -1;
    }

    // No index corresponds to available semaphore at first
    int index = -1;
    for (int i = 0; i < M; i++)
    {
        // If true then semaphore with index i is available for use
        if (sem_indexes[i] != -1)
        {
            // Store i and exit loop
            index = i;
            break;
        }
    }
    free(sem_indexes);

    // Return first available semaphore index
    return index;
}