Simple Shell

This is a simple UNIX command line interpreter (hsh) I made for Holberton School. It works kind of like sh, just simpler.


What it does

-runs in interactive mode, shows a ($) prompt and lets you type commands
-also works if you pipe stuff into it, like: echo "/bin/ls" | ./hsh
-it can run commands that are on PATH, like ls, pwd, etc
-it can also run commands with a full path or relative path, like /bin/ls or ./my_program
-handles commands with arguments, like /bin/ls -l /tmp
-has exit, to quit the shell
-has env, to print the environment variables
-keeps track of the last command's exit status and uses that as the shell's own exit status


How to compile

gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh


How to use it

Interactive:

$ ./hsh ($) /bin/ls main.c shell.c shell.h README.md ($) exit $



Non interactive:

$ echo "/bin/ls" | ./hsh

or

$ ./hsh < some_file_with_commands


How it works (basically)

1. get_line reads a line of input (and prints the prompt first, but only if it's interactive)
2. split_line breaks that line up into words
3. check_build checks if it's exit or env, and if so just runs it directly, no need to fork
4. find_command figures out the full path of the command, either because it already has a / in it, or by searching through PATH
5. if it can't find the command anywhere, it prints an error like sh does and exits with 127
6. if it does find it, fork_exec forks a child process and uses execve to actually run it, then waits for it to finish


Files

shell.c - the main loop, reads input and figures out what to do with it main.c - has find_command, search_path and the env builtin shell.h - header file with the function prototypes man_1_simple_shell - man page AUTHORS - who worked on this


Author

Jean Carlos Jimenez Nevarez