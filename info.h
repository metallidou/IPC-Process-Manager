#ifndef INFO_H
#define INFO_H

#include <semaphore.h>

#define LINE 256

typedef struct ProcessInfo
{
    char name[5];               // Process name
    int process_index;          // Index that corresponds to process index in struct ProcessInfo
    pid_t pid;                  // Pid of child process
    int sem_index;              // Index that corresponds to the index of the semaphore on the semaphores array (if -1 then no semaphore assigned)
    int active;                 // 1 if active, 0 otherwise
} ProcessInfo;

typedef struct Processes
{
    int n;                      // Number of total processes in config file (according to name)
    int end_timestamp;          // EXIT timestamp
    ProcessInfo* info;          // Pointer to struct for additional process info
} Processes; 

typedef struct ConfigInfo
{
    int timestamp;              // Timestamp on current line
    char process_name[5];       // Process name
    char command;               // Command S or T
    int process_index;          // Index that corresponds to process index in struct ProcessInfo
} ConfigInfo;

typedef struct Config
{
    int n;                      // Number of lines in config file
    char text_file[LINE];       // Text file to read from
    ConfigInfo* info;           // Pointer to struct for every config line info
} Config;

#endif