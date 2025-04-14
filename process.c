#include <string.h>
#include <stdlib.h>
#include "process.h"

void initialize_processes(Processes* processes, char* config_file)
{
    // Get number of config file lines
    int n = count_file_lines(config_file);

    // Initialize processes data
    processes->n = 0;
    processes->end_timestamp = 0;

    // Allocate memory for processes->info and initialize members
    processes->info = malloc(n * sizeof(ProcessInfo));
    for (int i = 0; i < n; i++)
    {
        strcpy(processes->info[i].name, "");
        processes->info[i].process_index = -1;
        processes->info[i].pid = -1;
        processes->info[i].sem_index = -1;
        processes->info[i].active = 0;
    }
}

int is_process_defined(Processes* processes, char* process_name)
{
    for (int i = 0; i < processes->n; i++)
    {
        // If process name is contained in processes->info then process is defined
        if (strcmp(processes->info[i].name, process_name) == 0)
            return 1;
    }
    return 0;
}

int get_process_index(Processes* processes, char* process_name)
{
    for (int i = 0; i < processes->n; i++)
    {
        // If names match process is found and index is returned 
        if (strcmp(processes->info[i].name, process_name) == 0)
            return processes->info[i].process_index;
    }
    return -1;
}