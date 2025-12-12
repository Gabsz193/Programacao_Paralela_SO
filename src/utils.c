#include "utils.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

double tempo_segundos() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

//< Imprime uma mensagem padrão de uso do programa.
void imprimir_uso(const char* prog_name) {
    fprintf(stderr, "Uso: %s --size <N> --threads [T] --mode <seq|par> --seed [S]\n", prog_name);
}

Args validar_argumentos(int argc, char* argv[]) {
    Args args = {
        .size = 0,
        .threads = 1,
        .mode = SEQ,
        .seed = time(NULL),
    };
    
    // Analisar argumentos da linha de comando 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            args.size = atoll(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            args.threads = atoi(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            const char* mode = argv[++i];
            
            // Converter o modo de string para enumerador
            if (strcmp(mode, "seq") == 0) {
                args.mode = SEQ;
            } else if (strcmp(mode, "par") == 0) {
                args.mode = PAR;
            } else {
                fprintf(stderr, VERMELHO("ERRO") "\tModo inválido: %s\n", mode);
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            args.seed = atoi(argv[i+1]);
            i++;
        } else {
            imprimir_uso(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    // Não permitir menos que 1 thread
    args.threads = (args.threads < 1) ? 1 : args.threads;

    // Verificar se o tamanho do vetor/matriz foi passado
    if (args.size == 0) {
        imprimir_uso(argv[0]);
        exit(EXIT_FAILURE);
    }

#ifndef NDEBUG
    // Imprimir valores para debugging
    fprintf(stderr, VERDE("DEBUG") "\tSize: %lu\n", args.size);
    fprintf(stderr, VERDE("DEBUG") "\tMode: %s\n", args.mode == SEQ ? "seq" : "par");
    fprintf(stderr, VERDE("DEBUG") "\tThreads: %d\n", args.threads);
#endif

    srand(args.seed);
    return args;
}