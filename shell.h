#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

extern char **environ;

char *find_command(char *command);
void print_env(void);
void get_line(char **line, size_t *len, int last_status);
void split_line(char *line, char **cmd_argv);
int check_build(char **cmd_argv, char *line, int last_status);
int fork_exec(char **cmd_argv, char *full_path);

#endif
