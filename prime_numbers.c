#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAX 200000
#define THREADS 16

int total_primes = 0;
pthread_mutex_t lock;

// Function to check if a number is prime
int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

// Structure for thread arguments
typedef struct {
    int start;
    int end;
} ThreadData;

// Thread function
void* count_primes(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int local_count = 0;

    for (int i = data->start; i <= data->end; i++) {
        if (is_prime(i)) {
            local_count++;
        }
    }

    // Update global count safely
    pthread_mutex_lock(&lock);
    total_primes += local_count;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[THREADS];
    ThreadData thread_data[THREADS];

    pthread_mutex_init(&lock, NULL);

    int chunk = MAX / THREADS;

    // Create threads
    for (int i = 0; i < THREADS; i++) {
        thread_data[i].start = i * chunk + 1;
        thread_data[i].end = (i == THREADS - 1) ? MAX : (i + 1) * chunk;

        if (pthread_create(&threads[i], NULL, count_primes, &thread_data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("The synchronized total number of prime numbers between 1 and 200,000 is %d\n", total_primes);

    return 0;
}
