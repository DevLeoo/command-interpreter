#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int pega_pipe_posicao(char **argv)
{
	int i = 0;
	while (argv[i])
	{
		if (argv[i][0] == '|')
		{
			return i;
		}
		i++;
	}
	return -1;
}

void imprime_argv(char **argv)
{
	int i = 0;
	if (argv[i])
		printf("===\n");
	while (argv[i])
	{
		printf("%d: '%s'\n", i, argv[i]);
		i++;
	}
	return;
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("Uso: %s <cmd> <p1> ... <pn> \n", argv[0]);
		return 0;
	}

	int pipe_position;
	pipe_position = pega_pipe_posicao(argv);

	printf("%d pipe \n", pipe_position);
	argv[pipe_position] = NULL;

	char **cmd1, **cmd2;
	cmd1 = &argv[1];
	cmd2 = &argv[pipe_position + 1];

	imprime_argv(cmd1);
	imprime_argv(cmd2);

	int fd[2];
	pid_t p_id;
	pipe(fd); // create a pipe
	p_id = fork();

	if (p_id == 0)
	{
		close(fd[READ_END]);				// close the write end of the pipe
		dup2(fd[WRITE_END], STDOUT_FILENO); // redirect stdin to the pipe
		close(fd[WRITE_END]);				// close the read end of the pipe
		execvp(cmd2[0], cmd2);
		exit(1); // exit if execvp fails
	}
	else
	{
		// parent
		close(fd[WRITE_END]);			  // close the read end of the pipe
		dup2(fd[READ_END], STDIN_FILENO); // redirect stdout to the pipe
		close(fd[READ_END]);			  // close the write end of the pipe
		execvp(cmd1[0], cmd1);
		exit(1); // exit if execvp fails
	}

	return 0;
}