#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "semaphores.h"

#define SHM_SIZE 1024
#define SHM_NAME "/shared_memory"

char* create_shared_memory();

#endif