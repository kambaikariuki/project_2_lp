#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define MAX_FILES 1000
#define BUFFER_SIZE 8192

pthread_mutex_t file_lock;

// Structure for thread arguments
typedef struct {
    char *filename;
    char *keyword;
    FILE *output;
} ThreadData;

// Count occurrences of keyword in a buffer
int count_occurrences(const char *buffer, const char *keyword) {
    int count = 0;
    int len = strlen(keyword);

    const char *ptr = buffer;
    while ((ptr = strstr(ptr, keyword)) != NULL) {
        count++;
        ptr += len;
    }
    return count;
}

// Thread function
void* search_file(void *arg) {
    ThreadData *data = (ThreadData*)arg;

    FILE *fp = fopen(data->filename, "r");
    if (!fp) {
        perror("fopen");
        pthread_exit(NULL);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes;
    int total_count = 0;

    // Read file in chunks
    while ((bytes = fread(buffer, 1, BUFFER_SIZE - 1, fp)) > 0) {
        buffer[bytes] = '\0';  // null terminate
        total_count += count_occurrences(buffer, data->keyword);
    }

    fclose(fp);

    // Write result to shared output file (synchronized)
    pthread_mutex_lock(&file_lock);
    fprintf(data->output, "File: %s, Count: %d\n", data->filename, total_count);
    pthread_mutex_unlock(&file_lock);

    pthread_exit(NULL);
}

// Timing helper
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s keyword output.txt file1 file2 ... threads\n", argv[0]);
        return 1;
    }

    char *keyword = argv[1];
    char *output_file = argv[2];
    int num_threads = atoi(argv[argc - 1]);

    int num_files = argc - 4;

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("fopen output");
        return 1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[MAX_FILES];

    pthread_mutex_init(&file_lock, NULL);

    double start = get_time();

    int thread_index = 0;

    // Assign one thread per file (limited by num_threads)
    for (int i = 0; i < num_files; i++) {
        thread_data[i].filename = argv[i + 3];
        thread_data[i].keyword = keyword;
        thread_data[i].output = output;

        pthread_create(&threads[thread_index], NULL, search_file, &thread_data[i]);
        thread_index++;

        // If max threads reached, join and reuse
        if (thread_index == num_threads) {
            for (int j = 0; j < thread_index; j++) {
                pthread_join(threads[j], NULL);
            }
            thread_index = 0;
        }
    }

    // Join remaining threads
    for (int i = 0; i < thread_index; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time();

    pthread_mutex_destroy(&file_lock);
    fclose(output);

    printf("Execution time with %d threads: %.6f seconds\n", num_threads, end - start);

    return 0;
}
