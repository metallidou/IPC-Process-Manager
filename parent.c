#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include "parent.h"

void parent_process(Config* config, Processes* processes, int M) 
{
    // Create shared memory segment for communication between parent and child processes
    char* shared_memory = create_shared_memory();
    
    // Create an array of semaphores to synchronize child processes
    sem_t** semaphores = create_semaphores_array(M);

    // Create semaphore to manage printing
    sem_t* sem_print = create_semaphore_print();

    // Initialize current time for the process simulation
    int current_time = 0;
    // Track the number of active child processes
    int active_children = 0;
    // Index for the config file, used to process each command line
    int i = 0;

    // Loop while exit timestamp is not reached
    while (current_time < processes->end_timestamp)
    {
        if (active_children > 0)
            sem_wait(sem_print);
        printf("Timestamp %d\n", current_time);
        
        // Check if there's a command to process at the current timestamp
        if (current_time == config->info[i].timestamp)
        {
            // Get the process index and command from the config
            int index = config->info[i].process_index;
            char command = config->info[i].command;

            // Handle the spawn command to create a new child process (if needed)
            if (command == 'S') 
            {
                // Spawn if active children are less than the total semaphores and process isn't active yet
                if (active_children < M && !processes->info[index].active) 
                {
                    // Assign available semaphore to child process 
                    int sem_index = assign_semaphore_to_process(*processes, index, M);
                    processes->info[index].sem_index = sem_index;
                    // Spawn the child process and store its pid
                    processes->info[index].pid = spawn_child(semaphores[sem_index], sem_print, shared_memory, processes->info[index].name, current_time);
                    processes->info[index].active = 1;
                    active_children++;
                }
                // Move to the next command in the config file
                i++;
            } 
            // Handle the terminate command to terminate an existing child process
            else if (command == 'T') 
            {
                // Only terminate if the child process is active
                if (processes->info[index].active) 
                {
                    // Retrieve the semaphore and PID of the child process to be terminated
                    int sem_index = processes->info[index].sem_index;
                    terminate_child(semaphores[sem_index], processes->info[index].pid, shared_memory, processes->info[index].name, current_time);
                    // Clear the semaphore index
                    processes->info[index].sem_index = -1;
                    processes->info[index].active = 0;
                    active_children--;
                }
                i++;
            }
        }
        // Increment the current timestamp for the next loop iteration
        current_time++;

        // Then the parent has nothing to do 
        if (active_children == 0)
            continue;

        // Choose a random active child process and select a random line from the text file
        int random_index = choose_random_active_child(*processes, active_children);
        char* random_line = choose_random_file_line(config->text_file);

        // Copy the selected random line to shared memory for the child to process
        strncpy(shared_memory, random_line, SHM_SIZE);
        free(random_line);

        // Retrieve the semaphore index for the chosen child and signal the child process to proceed
        int sem_index = processes->info[random_index].sem_index;
        sem_post(semaphores[sem_index]); 
    }

    // After the exit timestamp is reached, terminate any remaining active child processes
    for (int i = 0; i < processes->n; i++)
    {
        // Check each child process to see if it's still active
        if (processes->info[i].active)
        {
            // Retrieve the semaphore and pid for the child to be terminated
            int sem_index = processes->info[i].sem_index;
            terminate_child(semaphores[sem_index], processes->info[i].pid, shared_memory, processes->info[i].name, current_time);
            // Clear the semaphore index
            processes->info[i].sem_index = -1;
            processes->info[i].active = 0;
            active_children--;
        }
    }

    // Cleanup the semaphores and shared memory
    cleanup_semaphores_array(semaphores, M);
    cleanup_semaphore_print(sem_print);

    // Unmap the shared memory and unlink it to remove the shared memory object
    munmap(shared_memory, SHM_SIZE);
    shm_unlink(SHM_NAME);
}

int choose_random_active_child(Processes processes, int active_processes)
{
    // Initialize active_count to keep track of how many processes are active
    int active_count = 0;

    // Allocate memory to store the indices of active processes
    int* active_indices = malloc(active_processes * sizeof(int));

    // Initialize active_indices array with active processes 
    for (int i = 0; i < processes.n; i++) 
    {
        // If the process is active, add its index to the active_indices array
        if (processes.info[i].active) 
        {
            active_indices[active_count++] = i;
        }
    }

    // Randomly select one of the active processes from the active_indices array
    int random_index = rand() % active_count;  

    // Get the index of the randomly chosen active process
    int chosen_process = active_indices[random_index];
    free(active_indices);

    // Return the chosen process index
    return chosen_process;
}
