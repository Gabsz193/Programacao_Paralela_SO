#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "utils.h"
#include "scalar_product.h"

void print_usage(const char* prog_name) {
    fprintf(stderr, "Uso: %s --size <N> --threads [T] --mode <seq|par> --seed [S]\n", prog_name);
}

int main(int argc, char* argv[]) {
    unsigned int seed = time(NULL);
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
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = atoi(argv[i+1]);
            i++;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Não permitir menos que 1 thread
    threads = threads < 1 ? 1 : threads;

    // Verificar se todos os parâmetros foram inseridos
    if (n == 0 || strlen(mode) == 0) {
        print_usage(argv[0]);
        return 1;
    }
    
    srand(seed);
    
    int* v1 = generate_vector(n, -100, 100);
    int* v2 = generate_vector(n, -100, 100);
    
    if (!v1 || !v2) {
        fprintf(stderr, VERMELHO("ERRO") "\tFalha ao alocar vetores (n = %lu)\n", n);
        free(v1);
        free(v2);
        return 1;
    }
    
    long long result = 0;
    double start_time = current_time_seconds();
    
    if (strcmp(mode, "seq") == 0) {
        result = produto_escalar_seq(v1, v2, n);
    } else if (strcmp(mode, "par") == 0) {
        result = produto_escalar_par(v1, v2, n, threads);
    } else {
        fprintf(stderr, VERMELHO("ERRO") "\tModo inválido: %s\n", mode);
        free(v1);
        free(v2);
        return 1;
    }
    
    double end_time = current_time_seconds();
    double elapsed = end_time - start_time;
    
    // Gerar saída em formato CSV para automatizar a execução
    printf("%s,%zu,%d,%.6f,%lld\n", mode, n, threads, elapsed, result);
    
    free(v1);
    free(v2);
    
    return 0;
}
