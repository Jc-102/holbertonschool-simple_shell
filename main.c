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
 * Return: a malloc'ed full path to the command if found,
 * or NULL if not found
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

	if (path_env == NULL)
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
 * main - entry point for the simple shell
 *
 * Return: 0 on success
 */

int main(void)
{
	char *line;
	char *argv[64];
	char *token;
	char *full_path;
	size_t len;
	ssize_t read;
	pid_t child_pid;
	int status;
	int i;

	line = NULL;
	len = 0;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("($) ");

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			free(line);
			exit(0);
		}

		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		i = 0;
		token = strtok(line, " \t");
		while (token != NULL && i < 63)
		{
			argv[i] = token;
			i++;
			token = strtok(NULL, " \t");
		}
		argv[i] = NULL;

		if (argv[0] == NULL)
			continue;

		full_path = find_command(argv[0]);
		if (full_path == NULL)
		{
			fprintf(stderr, "%s: not found\n", argv[0]);
			continue;
		}

		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Error");
			free(full_path);
			continue;
		}

		if (child_pid == 0)
		{
			execve(full_path, argv, environ);
			perror("Error");
			free(full_path);
			exit(1);
		}
		else
		{
			wait(&status);
			free(full_path);
		}
	}

	return (0);
}
