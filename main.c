#include "main.h"

extern char **environ;

int main(int ac, char **argv)
{
    char *prompt = "Shell$ ";
    char *inputptr = NULL, *input_copy = NULL;
    size_t e = 0;
    ssize_t nread;
    const char *delim = " \n";
    char *token;
    int i = 0;
    int tok_num = 0;

    (void)ac;
    (void)argv;

    while (1)
    {
        printf("%s", prompt);

        nread = getline(&inputptr, &e, stdin);
        input_copy = malloc(sizeof(char) * nread);

        if (input_copy == NULL)
        {
            perror("tsh: memory allocation error");
            return (-1);
        }

        strcpy(input_copy, inputptr);

        if (nread == -1)
        {
            printf("Exiting\n");
            free(input_copy);
            free(inputptr);
            exit(0);
        }
        else
        {
            token = strtok(inputptr, delim);
            while (token != NULL)
            {
                tok_num++;
                token = strtok(NULL, delim);
            }
            tok_num++;

            argv = malloc(sizeof(char *) * tok_num);

            token = strtok(input_copy, delim);

            for (i = 0; token != NULL; i++)
            {
                argv[i] = malloc(sizeof(char) * strlen(token));
                strcpy(argv[i], token);

                token = strtok(NULL, delim);
            }
        }
        argv[i] = NULL;

        if (strcmp(argv[0], "exit") == 0)
        {
            printf("Exiting shell.\n");
            free(argv);
            free(input_copy);
            free(inputptr);
            exit(0);
        }
        else if (strcmp(argv[0], "env") == 0)
        {
            char *env_var = *environ;
            while (env_var)
            {
                printf("%s\n", env_var);
                env_var = *(environ++);
            }
        }
        else
        {

            char *command = argv[0];
            char *path = getenv("PATH");
            char *path_token = strtok(path, PATH_DELIMITER);
            int executed = 0;

            while (path_token)
            {
                char *executable_path = malloc(strlen(path_token) + strlen(command) + 2);
                sprintf(executable_path, "%s/%s", path_token, command);

                if (access(executable_path, X_OK) == 0)
                {
                    executed = 1;
                    if (fork() == 0)
                    {
                        execve(executable_path, argv, NULL);
                        perror("Error");
                        exit(EXIT_FAILURE);
                    }
                    wait(NULL);
                    free(executable_path);
                    break;
                }
                free(executable_path);
                path_token = strtok(NULL, PATH_DELIMITER);
            }

            if (!executed)
            {
                printf("Command not found: %s\n", command);
            }
        }

        free(argv);
        free(input_copy);
        free(inputptr);
    }

    return (0);
}
