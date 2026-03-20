#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 65536

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

    FILE *src = fopen(argv[1], "rb");
    FILE *dest = fopen(argv[2], "wb");

    if (!src || !dest) {
        perror("File open error");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;

    double start = get_time();

    while ((bytes = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    double end = get_time();

    fclose(src);
    fclose(dest);

    printf("Standard I/O copy time: %.6f seconds\n", end - start);
    return 0;
}
