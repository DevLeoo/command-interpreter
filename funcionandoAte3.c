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
    int fd[2];
    pid_t p_id;

    char **cmd1, **cmd2;
    cmd1 = &argv[1];

    printf("CMD 1\n");
    imprime_argv(cmd1);

    while ((pipe_position = pega_pipe_posicao(cmd1)) != -1)
    {
        cmd1[pipe_position] = NULL;

        if (pipe(fd) == -1)
        {
            perror("pipe");
            exit(1);
        }

        p_id = fork();

        if (p_id == 0)
        {
            close(fd[WRITE_END]);
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[READ_END]);

            cmd2 = &cmd1[pipe_position + 1];

            printf("CMD 2\n");
            imprime_argv(cmd2);
            execvp(cmd2[0], cmd2);
            perror("execvp");
            exit(1);
        }
        else
        {
            // parent
            close(fd[READ_END]);
            dup2(fd[WRITE_END], STDOUT_FILENO);
            close(fd[WRITE_END]);

            printf("CMD 1\n");
            imprime_argv(cmd1);
            execvp(cmd1[0], cmd1);
            perror("execvp");
            exit(1);
        }

        cmd1 = &cmd1[pipe_position + 1];
    }

    // last command
    imprime_argv(cmd1);
    execvp(cmd1[0], cmd1);
    perror("execvp");
    exit(1);
}