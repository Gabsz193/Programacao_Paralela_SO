#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "scalar_product.h"

void print_usage(const char* prog_name) {
    fprintf(stderr, "Usage: %s --size <N> --threads <T> --mode <seq|par>\n", prog_name);
}

int main(int argc, char* argv[]) {
    size_t n = 0;
    int threads = 1;
    char mode[10] = "";
    
    // Simple argument parsing
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            n = atoll(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            threads = atoi(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            strncpy(mode, argv[i+1], 9);
            mode[9] = '\0';
            i++;
        }
    }
    
    if (n == 0 || strlen(mode) == 0) {
        print_usage(argv[0]);
        return 1;
    }
    
    srand(time(NULL));
    
    int* v1 = generate_vector(n, -100, 100);
    int* v2 = generate_vector(n, -100, 100);
    
    if (!v1 || !v2) {
        fprintf(stderr, "Memory allocation failed\n");
        free(v1);
        free(v2);
        return 1;
    }
    
    long long result = 0;
    double start_time = current_time_seconds();
    
    if (strcmp(mode, "seq") == 0) {
        result = scalar_product_seq(v1, v2, n);
    } else if (strcmp(mode, "par") == 0) {
        result = scalar_product_par(v1, v2, n, threads);
    } else {
        fprintf(stderr, "Invalid mode: %s\n", mode);
        free(v1);
        free(v2);
        return 1;
    }
    
    double end_time = current_time_seconds();
    double elapsed = end_time - start_time;
    
    // Output format: Mode, Size, Threads, Time, Result
    // Using CSV-like format for easy parsing
    printf("%s,%zu,%d,%.6f,%lld\n", mode, n, threads, elapsed, result);
    
    free(v1);
    free(v2);
    
    return 0;
}
