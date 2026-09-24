#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * main - entry point for the simple shell
 *
 * Return: 0 on success
 */

int main(void)
{
	char *line;
	char *argv[2];
	size_t len;
	ssize_t read;
	pid_t child_pid;
	int status;

	line = NULL;
	len = 0;

	while (1)
	{
		printf("($) ");

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			printf("\n");
			free(line);
			exit(0);
		}

		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		argv[0] = line;
		argv[1] = NULL;

		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Error");
			continue;
		}

		if (child_pid == 0)
		{
			if (execve(argv[0], argv, environ) == -1)
			{
				perror("Error");
				exit(1);
			}
		}
		else
		{
			wait(&status);
		}
	}

	return (0);
}
