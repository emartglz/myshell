#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#define BUFFERSIZE 4096

int pipe_father(int pipefd[])
{
    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);
    read(pipefd[0], buffer, BUFFERSIZE);
    char *tokenize = strtok(buffer, "\n");
    int c = 1;
    while(tokenize)
    {
        printf("%d %s \n", c, tokenize);
        tokenize = strtok(NULL, "\n");
        c++;
    }
}

int pipe_child(int argc, char **argv, int pipefd1[], int pipefd2[])
{
    char *params[argc];
    for(int i = 0; i < argc; i++)
    {
        params[i] = argv[i + 1];
    }
    params[argc - 1] = NULL;

    dup2(STDIN_FILENO, pipefd1[0]);

    dup2(pipefd2[1], STDOUT_FILENO);
    
    close(pipefd2[1]);
    int exec_ret = execvp(params[0], params);
}



int pipes(int c, int *argc, char ***argv)
{
    int pipefd1[2];
    pipe(pipefd1);
    dup2(pipefd1[1], STDIN_FILENO);

    for(int i = 0; i < c; i++)
    {
        int pipefd2[2];
        pipe(pipefd2);

        int pid = fork();
        if(pid)
        {
            if(i == c - 1)
            {
                pipe_father(pipefd2);
            }
        }
        else
        {
            pipe_child(argc[i], argv[i], pipefd1, pipefd2);
            return 0;
        }
        pipefd1[0] = pipefd2[0];
        pipefd1[1] = pipefd2[1];
    }

    dup2(STDOUT_FILENO, pipefd1[0]);

    return 0;
}

int main(int argc, char **argv)
{
    int a[1];
    char **b[1];
    a[0] = argc;
    b[0] = argv;

    pipes(1, a, b);
    return 0;
}