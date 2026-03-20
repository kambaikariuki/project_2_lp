#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pipefd[2];
    pid_t pid1, pid2;

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // First child (ps aux)
    pid1 = fork();
    if (pid1 == 0) {
        // Redirect stdout to pipe write end
        dup2(pipefd[1], STDOUT_FILENO);

        // Close unused ends
        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"ps", "aux", NULL};
        execvp(args[0], args);

        perror("execvp ps");
        exit(EXIT_FAILURE);
    }

    // Second child (grep root)
    pid2 = fork();
    if (pid2 == 0) {
        // Redirect stdin from pipe read end
        dup2(pipefd[0], STDIN_FILENO);

        // Open file for output
        int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to file
        dup2(fd, STDOUT_FILENO);

        // Close unused descriptors
        close(pipefd[1]);
        close(pipefd[0]);
        close(fd);

        char *args[] = {"grep", "root", NULL};
        execvp(args[0], args);

        perror("execvp grep");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for children
    wait(NULL);
    wait(NULL);

    // Read part of output file
    FILE *file = fopen("output.txt", "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("---- Partial Output ----\n");

    char buffer[256];
    int count = 0;

    while (fgets(buffer, sizeof(buffer), file) && count < 5) {
        printf("%s", buffer);
        count++;
    }

    fclose(file);

    return 0;
}
