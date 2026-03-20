#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 4096

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s source dest\n", argv[0]);
        return 1;
    }

    int src = open(argv[1], O_RDONLY);
    int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (src < 0 || dest < 0) {
        perror("File open error");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes;
    
    double start = get_time();

    while ((bytes = read(src, buffer, BUFFER_SIZE)) > 0) {
        write(dest, buffer, bytes);
    }

    double end = get_time();

    close(src);
    close(dest);

    printf("Low-level copy time: %.6f seconds\n", end - start);
    return 0;
}
