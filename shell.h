#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

extern char **environ;

char *find_command(char *command);
void print_env(void);

#endif