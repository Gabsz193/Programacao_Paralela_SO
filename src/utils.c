#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int* generate_vector(size_t n, int min, int max) {
    int* v = (int*)malloc(n * sizeof(int));
    if (!v) {
        perror("Falha ao alocar vetor");
        return NULL;
    }
    
    /**
     * Deixaremos a responsabilidade de executar `srand` ao usuário
     * da função para permitir a reprodutibilidade dos experimentos.
     */

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
