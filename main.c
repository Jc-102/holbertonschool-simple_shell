#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * trim_line - removes leading and trailing whitespace from a string
 * @line: the string to trim, modified in place
 *
 * Return: pointer to the start of the trimmed string
 */

char *trim_line(char *line)
{
	char *start;
	char *end;

	start = line;
	while (*start == ' ' || *start == '\t')
		start++;

	if (*start == '\0')
		return (start);

	end = start + strlen(start) - 1;
	while (end > start && (*end == ' ' || *end == '\t'))
		end--;

	*(end + 1) = '\0';

	return (start);
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
