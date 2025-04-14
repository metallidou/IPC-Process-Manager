#ifndef FILE_H
#define FILE_H

#include "info.h"
#include "process.h"

int count_file_lines(char* file_name);
void read_config(char* config_file, char* text_file, Config* config, Processes* processes);
void initialize_config(Config* config, char* config_file, char* text_file);
char* choose_random_file_line(char* file_name);

#endif 