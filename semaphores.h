#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <semaphore.h>
#include "process.h"

sem_t** create_semaphores_array(int M);
sem_t* create_semaphore_print();
void cleanup_semaphores_array(sem_t** semaphores, int M);
void cleanup_semaphore_print(sem_t* sem_print);
int assign_semaphore_to_process(Processes process, int process_index, int M);

#endif