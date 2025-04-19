#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "parent.h"

int main(int argc, char** argv) 
{
    // Arguments must be: config file (filepath), text file (text_file), number of semaphores (M)
    if (argc < 4) 
    {
        fprintf(stderr, "You have inserted less arguments than needed\n");
        return 1; 
    }
    
    // Store config file
    char* config_file = malloc(strlen(argv[1]) + 1);
    if (!config_file) 
    {
        perror("malloc");
        return 1;
    }
    strcpy(config_file, argv[1]);

    // Store text file
    char* text_file = malloc(strlen(argv[2]) + 1);
    if (!text_file) 
    {
        perror("malloc");
        return 1;
    }
    strcpy(text_file, argv[2]);

    // Store number of semaphores that can be used
    int M = atoi(argv[3]);

    // Create config struct pointer to store config file data 
    Config* config = malloc(sizeof(Config));
    if (!config) 
    {
        perror("malloc");
        free(config_file);
        return 1;
    }

    // Create processes struct pointer to store each process' data
    Processes* processes = malloc(sizeof(Processes));
    if (!processes) 
    {
        perror("malloc");
        free(config);
        free(config_file);
        return 1;
    }

    printf("CONFIG %s\n", config_file);
    printf("TEXT %s\n", text_file);

    // Read config file and initialize config and processes 
    read_config(config_file, text_file, config, processes);
    
    // Start the parent process  
    parent_process(config, processes, M);


    // Free
    free(config_file); 

    free(config->info);
    free(config);

    free(processes->info);
    free(processes);

    return 0;
}