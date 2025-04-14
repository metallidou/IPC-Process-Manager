#ifndef CHILD_H
#define CHILD_H

#include <semaphore.h>
#include "info.h"

pid_t spawn_child(sem_t* sem, sem_t* sem_print, char* shared_memory, char* child_name, int timestamp);
void terminate_child(sem_t* sem, pid_t pid, char* shared_memory, char* child_name, int timestamp);
void child_process(sem_t* sem, sem_t* sem_print, char* shared_memory, char* child_name);

#endif