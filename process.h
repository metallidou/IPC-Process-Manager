#ifndef PROCESS_H
#define PROCESS_H

#include "file.h"

void initialize_processes(Processes* processes, char* config_file);
int is_process_defined(Processes* processes, char* process_name);
int get_process_index(Processes* processes, char* process_name);

#endif