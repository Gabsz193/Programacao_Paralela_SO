#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "utils.h"
#include "scalar_product.h"

int main(int argc, char* argv[]) {
    Args args = validar_argumentos(argc, argv);
    
    // Gerar os vetores e verificar a alocação
    int* v1 = gerar_vetor(args.size, -100, 100);
    int* v2 = gerar_vetor(args.size, -100, 100);
    
    if (!v1 || !v2) {
        fprintf(stderr, VERMELHO("ERRO") "\tFalha ao alocar vetores (n = %lu)\n", args.size);
        free(v1);
        free(v2);
        return 1;
    }
    
    long long result = 0;
    double start_time = tempo_segundos();
    
    if (args.mode == SEQ) {
        result = produto_escalar_seq(v1, v2, args.size);
    } else if (args.mode == PAR) {
        result = produto_escalar_par(v1, v2, args.size, args.threads);
    }
    
    double end_time = tempo_segundos();
    double elapsed = end_time - start_time;
    
    // Gerar saída em formato CSV para automatizar a execução
    printf("%s,%zu,%d,%.6f,%lld\n", args.mode == SEQ ? "seq" : "par", args.size, args.threads, elapsed, result);
    
    free(v1);
    free(v2);
    return 0;
}
