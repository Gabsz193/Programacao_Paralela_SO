#include "matrix_product.h"
#include "log.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

//< Cria uma matriz com uma dimensão dada.
matriz_t *criar_matriz(size_t linhas, size_t colunas) {
    matriz_t *matriz = malloc(sizeof(matriz_t));
    matriz->dados = calloc(linhas * colunas, sizeof(int));
    matriz->linhas = linhas;
    matriz->colunas = colunas;
    return matriz;
};

//< Gera uma matriz com uma dimensão dada e valores aleatórios.
matriz_t *gerar_matriz(size_t linhas, size_t colunas, int min, int max) {
    matriz_t *matriz = criar_matriz(linhas, colunas);

    // Gerar valores aleatórios
    for (size_t i = 0; i < linhas; i++) {
        for (size_t j = 0; j < colunas; j++) {
            MAT_POS(matriz, i, j) = min + rand() % (max - min + 1);
        }
    }
    
    return matriz;
};

//< Imprime uma matriz no `stderr` com um rótulo dado.
void imprimir_matriz(const matriz_t *m, const char *nome) {
    fprintf(stderr, CIANO("INFO") "\tMatriz '%s' (%lux%lu):\n", nome, m->linhas, m->colunas);
    for (size_t i = 0; i < m->linhas; i++) {
        fprintf(stderr, CIANO("INFO") "\t| ");
        for (size_t j = 0; j < m->colunas; j++) {
            // Pular colunas caso hajam muitas
            if (j == 15) {
                fprintf(stderr, " ... ");
                j = m->colunas - 5;
            }
            fprintf(stderr, "%5d", MAT_POS(m, i, j));
        }
        fprintf(stderr, " |\n");
    }
};

//< Libera a memória associada à uma matriz.
void free_matriz(matriz_t* matriz) {
    free(matriz->dados);
    free(matriz);
}

typedef struct {
    //< A matriz à esquerda no produto.
    const matriz_t* a;
    
    //< A matriz à direita no produto.
    const matriz_t* b;

    //< Armazena o resultado do produto entre as matrizes.
    matriz_t* destino;
    
    //< O índice da linha de `destino` em que a função deve iniciar
    // o cálculo.
    size_t inicio;

    //< A quantidade de linhas para produzir em `destino`.
    size_t fim;
} ProdMatrizesInfo;

//< Verifica se é possível realizar o produto `a * b`, retornando 1 caso seja.
static int verificar_produto_matrizes(const matriz_t* a, const matriz_t* b) {
    return a && b && a->dados && b->dados && a->colunas == b->linhas;
}

//< Executa o produto entre duas matrizes garantidamente multiplicáveis.
void produto_matrizes(ProdMatrizesInfo* info) {
    for (size_t i = info->inicio; i < info->fim; i++) {
        for (size_t j = 0; j < info->destino->colunas; j++) {
            MAT_POS(info->destino, i, j) = 0;
            for (size_t k = 0; k < info->a->colunas; k++) {
                MAT_POS(info->destino, i, j) += (long long)(MAT_POS(info->a, i, k)) * MAT_POS(info->b, k, j);
            }
        }
    }
}

//< Utilitário para executar `produto_matrizes` com `pthread_create`.
void* produto_matrizes_thread(void* arg) {
    produto_matrizes((ProdMatrizesInfo*) arg);
    return NULL;
}

//< Calcula sequencialmente o produto `a * b` entre duas matrizes.
matriz_t *produto_matrizes_seq(const matriz_t* a, const matriz_t* b) {
    if (!verificar_produto_matrizes(a, b)) {
        fprintf(stderr, VERMELHO("ERRO") "\tNão é possível multiplicar as matrizes a (%p) e b (%p).", a, b);
        return NULL;
    }
    
    ProdMatrizesInfo info = {
        .a = a,
        .b = b,
        .destino = criar_matriz(a->linhas, b->colunas),
        .inicio = 0,
        .fim = a->linhas,
    };

    produto_matrizes(&info);
    return info.destino;
};

//< Calcula paralelamente o produto `a * b` entre duas matrizes, dado o número de threads.
matriz_t *produto_matrizes_par(const matriz_t* a, const matriz_t* b, int num_threads) {
    if (num_threads <= 0) return 0;

    if (!verificar_produto_matrizes(a, b)) {
        fprintf(stderr, VERMELHO("ERRO") "\tNão é possível multiplicar as matrizes a (%p) e b (%p).", a, b);
        return NULL;
    }
    
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ProdMatrizesInfo* thread_data = (ProdMatrizesInfo*)malloc(num_threads * sizeof(ProdMatrizesInfo));

    matriz_t* destino = criar_matriz(a->linhas, b->colunas);
    
    size_t linhas_por_thread = destino->linhas / num_threads;
    size_t resto = destino->linhas % num_threads;

    size_t inicio_segmento = 0;
    for (int i = 0; i < num_threads; i++) {
        ProdMatrizesInfo* data = &thread_data[i];

        data->a = a;
        data->b = b;
        data->destino = destino;
        data->inicio = inicio_segmento;
        data->fim = data->inicio + linhas_por_thread + ((size_t)i < resto);
        
        inicio_segmento += data->fim - data->inicio;
        
        if (pthread_create(&threads[i], NULL, produto_matrizes_thread, data) != 0) {
            perror("Falha ao criar thread");
            
            // Cancelar todas as threads já iniciadas
            for (int j = 0; j < i; j++)
                pthread_cancel(threads[j]);

            free_matriz(destino);
            free(threads);
            free(thread_data);
            return NULL; 
        }
    }
    
    // Aguardar todas as threads terminarem
    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);
    
    free(threads);
    free(thread_data);
    return destino;
};
