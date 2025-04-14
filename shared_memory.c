#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "shared_memory.h"

char* create_shared_memory()
{
    // Create shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        // Handle failure to create or open the shared memory object.
        perror("Shared memory creation failed");
        exit(1);
    }

    // Set the size of the shared memory object
    // This ensures the shared memory segment is large enough to store the required data
    if (ftruncate(shm_fd, SHM_SIZE) == -1) 
    {
        // Handle failure to resize the shared memory object
        perror("Shared memory resizing failed");
        exit(1);
    }

    // Memory map the shared memory object into the process's address space
    char* shared_memory = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) 
    {
        // Handle failure to map the shared memory object
        perror("Shared memory mapping failed");
        exit(1);
    }
    return shared_memory;
}