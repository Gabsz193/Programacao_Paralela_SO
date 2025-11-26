#include "scalar_product.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

long long scalar_product_seq(const int* v1, const int* v2, size_t n) {
    long long sum = 0;
    for (size_t i = 0; i < n; i++) {
        sum += (long long)v1[i] * v2[i];
    }
    return sum;
}

typedef struct {
    const int* v1;
    const int* v2;
    size_t start;
    size_t end;
    long long partial_sum;
} ThreadData;

void* scalar_product_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_sum = 0;
    for (size_t i = data->start; i < data->end; i++) {
        data->partial_sum += (long long)data->v1[i] * data->v2[i];
    }
    return NULL;
}

long long scalar_product_par(const int* v1, const int* v2, size_t n, int num_threads) {
    if (num_threads <= 0) return 0;
    
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    
    size_t chunk_size = n / num_threads;
    size_t remainder = n % num_threads;
    
    size_t current_start = 0;
    
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].v1 = v1;
        thread_data[i].v2 = v2;
        thread_data[i].start = current_start;
        
        size_t current_chunk = chunk_size + ((size_t)i < remainder ? 1 : 0);
        thread_data[i].end = current_start + current_chunk;
        current_start += current_chunk;
        
        if (pthread_create(&threads[i], NULL, scalar_product_thread, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            // Cleanup and return error or partial result? 
            // For simplicity, we exit or return 0. 
            // Ideally we should cancel previous threads.
            free(threads);
            free(thread_data);
            return 0; 
        }
    }
    
    long long total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }
    
    free(threads);
    free(thread_data);
    
    return total_sum;
}
