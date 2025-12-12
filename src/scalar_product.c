#include "scalar_product.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

//< Gera um vetor de tamanho `n` com números aleatórios entre `min` e `max`.
int* gerar_vetor(size_t n, int min, int max) {
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

typedef struct {
    //< O primeiro vetor do produto escalar.
    const int* v1;
    
    //< O segundo vetor do produto escalar.
    const int* v2;
    
    //< A quantidade de elementos de v1 e v2 para processar. O
    // tamanho de v1 e v2 deve ser maior ou igual a `tamanho`.
    size_t tamanho;

    //< Armazena o resultado do produto escalar após executar
    // a função `produto_escalar(...);`
    long long resultado;
} ProdEscalarInfo;

//< Executa o produto escalar entre dois vetores quaisquer.
void produto_escalar(ProdEscalarInfo* info) {
    info->resultado = 0;
    for (size_t i = 0; i < info->tamanho; i++) {
        info->resultado += (long long)(info->v1[i]) * info->v2[i];
    }
}

//< Utilitário para executar `produto_escalar` com `pthread_create`.
void* produto_escalar_thread(void* arg) {
    produto_escalar((ProdEscalarInfo*) arg);
    return NULL;
}

//< Calcula sequencialmente o produto escalar `v1 * v2` entre dois vetores.
long long produto_escalar_seq(const int* v1, const int* v2, size_t n) {
    ProdEscalarInfo info = {
        .v1 = v1,
        .v2 = v2,
        .tamanho = n,
        .resultado = 0
    };

    produto_escalar(&info);
    return info.resultado;
}

//< Calcula paralelamente o produto escalar `v1 * v2` entre dois vetores, dado um número de threads.
long long produto_escalar_par(const int* v1, const int* v2, size_t n, int num_threads) {
    if (num_threads <= 0) return 0;
    
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ProdEscalarInfo* thread_data = (ProdEscalarInfo*)malloc(num_threads * sizeof(ProdEscalarInfo));
    
    size_t tamanho_segmento = n / num_threads;
    size_t resto = n % num_threads;
    
    size_t inicio_segmento = 0;
    
    for (int i = 0; i < num_threads; i++) {
        ProdEscalarInfo* data = &thread_data[i];

        data->v1 = v1 + inicio_segmento;
        data->v2 = v2 + inicio_segmento;
        data->tamanho = tamanho_segmento + ((size_t)i < resto);
        
        inicio_segmento += data->tamanho;
        
        if (pthread_create(&threads[i], NULL, produto_escalar_thread, data) != 0) {
            perror("Falha ao criar thread");
            
            // Cancelar todas as threads já iniciadas
            for (int j = 0; j < i; j++)
                pthread_cancel(threads[j]);

            free(threads);
            free(thread_data);
            return 0; 
        }
    }
    
    // Aguardar todas as threads terminarem e somar seus resultados
    long long soma_total = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        soma_total += thread_data[i].resultado;
    }
    
    free(threads);
    free(thread_data);
    return soma_total;
}
