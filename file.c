#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

int count_file_lines(char* file_name)
{
    // Open file
    FILE* file = fopen(file_name, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open config file\n");
        return -1;
    }

    int count = 0;
    char line[LINE];

    // Count lines in file
    while (fgets(line, sizeof(line), file)) 
        count++;
    fclose(file);

    return count;
}

void read_config(char* config_file, char* text_file, Config* config, Processes* processes)
{
    // Open config file to read data
    FILE* file = fopen(config_file, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open config file\n");
        return;
    }

    // Initialize config 
    initialize_config(config, config_file, text_file);

    // Initialize processes
    initialize_processes(processes, config_file);
        
    // Config file data
    int timestamp;
    char name[5];
    char command;
    
    // Config file line
    char line[LINE];
    // Each process has an index that corresponds to its position on process->info array
    int index;
    // Process counter
    int p_count = 0;
    // Config file line counter
    int i = 0;

    // Read config file lines
    while (fgets(line, sizeof(line), file)) 
    {
        if (sscanf(line, "%d %s %c", &timestamp, name, &command) == 3)
        {
            // Extract line data: timestamp, process name, command (S/T)
            printf("Timestamp: %d, Process: %s, Command: %c\n", timestamp, name, command);
        } 
        else if (sscanf(line, "%d", &timestamp) == 1)
        {
            // Extract final line data: timestamp <EXIT>
            processes->end_timestamp = timestamp;
            printf("End timestamp: %d\n", timestamp);
            break;;
        }
        else
        {
            // Then invalid file format
            fprintf(stderr, "Invalid config file format\n");
        }

        if (!is_process_defined(processes, name))
        {
            // Add new process that has not be defined yet
            index = p_count;
            // Update processes data
            processes->n++;
            processes->info[index].process_index = index;
            strcpy(processes->info[index].name, name); 
            p_count++;
        }
        else
        {
            // Process with name has already been defined
            index = get_process_index(processes, name);
        }

        if (command != 'S' && command != 'T')
        {
            // Commands should only have the values S/T
            fprintf(stderr, "Invalid command on config file\n");
            return;
        }       

        // Update config data
        config->info[i].timestamp = timestamp; 
        strcpy(config->info[i].process_name, name);
        config->info[i].command = command;
        config->info[i].process_index = index;

        // Go to next config file line
        i++;
    }
    fclose(file);
}

void initialize_config(Config* config, char* config_file, char* text_file)
{
    // Get number of config file lines
    config->n = count_file_lines(config_file);

    // Assign text file 
    strcpy(config->text_file, text_file);

    // Allocate memory for config->info and initialize members
    config->info = malloc(config->n * sizeof(ConfigInfo));
    for (int i = 0; i < config->n; i++)
    {
        config->info[i].timestamp = 0;
        strcpy(config->info[i].process_name, "");
        config->info[i].command = ' ';
        config->info[i].process_index = -1;
    }
}

char* choose_random_file_line(char* file_name)
{
    // Open file
    FILE* file = fopen(file_name, "r");
    if (file == NULL) 
    {
        perror("Failed to open file");
        return NULL; 
    }

    size_t n = 0;
    // Selected line to return
    char selected[LINE]; 
    // Current file line
    char line[LINE];

    // Read file lines
    while (fgets(line, sizeof(line), file)) 
    {
        // Select a line with probability 1/n
        if (drand48() < 1.0 / ++n) 
            strcpy(selected, line);
    }
    fclose(file);

    // Check if the selected line is empty or contains only whitespace
    // If true then choose again
    if (strspn(selected, " \t\n\r") == strlen(selected)) 
        return choose_random_file_line(file_name);  

    // If selected line contains '\n' character at the end replace it with '\0' for printing purposes
    size_t length = strlen(selected);
    if (length > 0 && selected[length-1] == '\n') 
        selected[length-1] = '\0';
    
    // Duplicate the selected line into dynamically allocated memory to return
    return strdup(selected);
}