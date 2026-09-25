#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * find_command - searches PATH for an executable command
 * @command: the command name to search for
 *
 * Return: a malloc'ed full path to the command if found, or NULL
 */

char *find_command(char *command)
{
	char *path_env;
	char *path_copy;
	char *dir;
	char *full_path;
	int i;

	if (strchr(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}

	path_env = NULL;
	for (i = 0; environ[i] != NULL; i++)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
		{
			path_env = environ[i] + 5;
			break;
		}
	}

	if (path_env == NULL || path_env[0] == '\0')
		return (NULL);

	path_copy = strdup(path_env);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		full_path = malloc(strlen(dir) + strlen(command) + 2);
		if (full_path == NULL)
		{
			free(path_copy);
			return (NULL);
		}

		sprintf(full_path, "%s/%s", dir, command);

		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}

		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * print_env - prints the current environment, one variable per line
 */

void print_env(void)
{
	int i;

	for (i = 0; environ[i] != NULL; i++)
		printf("%s\n", environ[i]);
}

/**
 * get_line - prints a prompt if interactive and reads one line
 * @line: pointer to the getline buffer
 * @len: pointer to the buffer's allocated length
 * @last_status: current exit status, used if EOF is reached
 */

void get_line(char **line, size_t *len, int last_status)
{
	ssize_t read;

	if (isatty(STDIN_FILENO))
		printf("($) ");

	read = getline(line, len, stdin);

	if (read == -1)
	{
		if (isatty(STDIN_FILENO))
			printf("\n");
		free(*line);
		exit(last_status);
	}

	if ((*line)[read - 1] == '\n')
		(*line)[read - 1] = '\0';
}

/**
 * split_line - splits a line into an argument vector
 * @line: the line to split
 * @cmd_argv: array to fill with tokens (max 63 + NULL)
 */

void split_line(char *line, char **cmd_argv)
{
	char *token;
	int i;

	i = 0;
	token = strtok(line, " \t");
	while (token != NULL && i < 63)
	{
		cmd_argv[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	cmd_argv[i] = NULL;
}

/**
 * check_build - runs a builtin command if cmd_argv[0] matches one
 * @cmd_argv: the command and its arguments
 * @line: the getline buffer, freed before exiting
 * @last_status: the shell's last exit status
 *
 * Return: 1 if a builtin ran, 0 otherwise
 */
int check_build(char **cmd_argv, char *line, int last_status)
{
	if (strcmp(cmd_argv[0], "exit") == 0)
	{
		free(line);
		exit(last_status);
	}

	if (strcmp(cmd_argv[0], "env") == 0)
	{
		print_env();
		return (1);
	}

	return (0);
}

/**
 * fork_exec - forks and executes an external command
 * @cmd_argv: the command and its arguments
 * @full_path: full path to the executable (freed here)
 *
 * Return: the exit status of the child process
 */

int fork_exec(char **cmd_argv, char *full_path)
{
	pid_t child_pid;
	int status;

	child_pid = fork();
	if (child_pid == -1)
	{
		perror("Error");
		free(full_path);
		return (1);
	}

	if (child_pid == 0)
	{
		execve(full_path, cmd_argv, environ);
		perror("Error");
		free(full_path);
		exit(1);
	}

	wait(&status);
	free(full_path);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (1);
}

/**
 * main - entry point for the simple shell
 * @argc: argument count (unused)
 * @argv: argument vector, argv[0] is the shell's own name
 *
 * Return: exit status of the last command run
 */

int main(int argc, char *argv[])
{
	char *line;
	char *cmd_argv[64];
	char *full_path;
	size_t len;
	unsigned int line_number;
	int last_status;

	(void)argc;
	line = NULL;
	len = 0;
	line_number = 0;
	last_status = 0;

	while (1)
	{
		get_line(&line, &len, last_status);
		line_number++;

		split_line(line, cmd_argv);
		if (cmd_argv[0] == NULL)
			continue;

		if (check_build(cmd_argv, line, last_status))
			continue;

		full_path = find_command(cmd_argv[0]);
		if (full_path == NULL)
		{
			fprintf(stderr, "%s: %u: %s: not found\n",
					argv[0], line_number, cmd_argv[0]);
			last_status = 127;
			continue;
		}

		last_status = fork_exec(cmd_argv, full_path);
	}

	return (0);
}
