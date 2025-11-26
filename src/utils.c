#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int* generate_vector(size_t n, int min, int max) {
    int* v = (int*)malloc(n * sizeof(int));
    if (!v) {
        perror("Failed to allocate vector");
        return NULL;
    }
    
    // Use a fixed seed for reproducibility if needed, or time(NULL) for randomness
    // For this assignment, random is fine, but for debugging fixed seed is better.
    // We'll use time(NULL) in main or here. Let's stick to rand() here assuming srand called in main.
    
    for (size_t i = 0; i < n; i++) {
        v[i] = min + rand() % (max - min + 1);
    }
    return v;
}

void print_vector(const int* v, size_t n, size_t k) {
    if (k > n) k = n;
    printf("[");
    for (size_t i = 0; i < k; i++) {
        printf("%d", v[i]);
        if (i < k - 1) printf(", ");
    }
    if (k < n) printf(", ...");
    printf("]\n");
}

double current_time_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
