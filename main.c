#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * search_path - searches each directory in PATH for a command
 * @command: the command name to search for
 * @path_env: the PATH environment variable value
 *
 * Return: a malloc'ed full path to the command if found, or NULL
 */

char *search_path(char *command, char *path_env)
{
	char *path_copy;
	char *dir;
	char *full_path;

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
 * find_command - resolves a command to a full executable path
 * @command: the command name to search for
 *
 * Return: a malloc'ed full path to the command if found, or NULL
 */

char *find_command(char *command)
{
	char *path_env;
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

	return (search_path(command, path_env));
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
