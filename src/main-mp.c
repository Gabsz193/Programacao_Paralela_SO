#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "matrix_product.h"
#include "utils.h"
#include "scalar_product.h"

int main(int argc, char* argv[]) {
    Args args = validar_argumentos(argc, argv);
    
    // Gerar os vetores e verificar a alocação
    matriz_t* a = gerar_matriz(args.size, args.size, -10, 10);
    matriz_t* b = gerar_matriz(args.size, args.size, -10, 10);
    
    if (!a || !b) {
        fprintf(stderr, VERMELHO("ERRO") "\tFalha ao alocar matrizes (n = %lu)\n", args.size);
        free_matriz(a);
        free_matriz(b);
        return 1;
    }
    
    matriz_t* result = NULL;
    double start_time = tempo_segundos();
    
    if (args.mode == SEQ) {
        result = produto_matrizes_seq(a, b);
    } else if (args.mode == PAR) {
        result = produto_matrizes_par(a, b, args.threads);
    }
    
    double end_time = tempo_segundos();
    double elapsed = end_time - start_time;
    
    // Gerar saída em formato CSV para automatizar a execução
    printf("%s,%zu,%d,%.6f\n", args.mode == SEQ ? "seq" : "par", args.size, args.threads, elapsed);
    
#ifndef NDEBUG
    // Imprimir a matriz no stderr por debugging
    imprimir_matriz(a, "a");
    imprimir_matriz(b, "b");
    imprimir_matriz(result, "result");
#endif

    free_matriz(a);
    free_matriz(b);
    return 0;
}
