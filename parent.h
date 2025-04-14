#ifndef PARENT_H
#define PARENT_H

#include "info.h"
#include "file.h"
#include "semaphores.h"
#include "shared_memory.h"
#include "child.h"

void parent_process(Config* config, Processes* processes, int M);
int choose_random_active_child(Processes processes, int active_processes);

#endif