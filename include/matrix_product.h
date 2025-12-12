#ifndef MATRIX_PRODUCT_H
#define MATRIX_PRODUCT_H

#include <stddef.h>

typedef struct {
    int *dados;
    size_t linhas;
    size_t colunas;
} matriz_t;

//< Acessa a matriz `m` na linha `i` e coluna `j`.
#define MAT_POS(m, i, j) (m->dados[(i * m->colunas) + j])

//< Cria uma matriz com uma dimensão dada.
matriz_t *criar_matriz(size_t linhas, size_t colunas);

//< Gera uma matriz com uma dimensão dada e valores aleatórios.
matriz_t *gerar_matriz(size_t linhas, size_t colunas, int min, int max);

//< Imprime uma matriz no `stderr` com um rótulo dado.
void imprimir_matriz(const matriz_t *matriz, const char *nome);

//< Libera a memória associada à uma matriz.
void free_matriz(matriz_t* matriz);

//< Calcula sequencialmente o produto `a * b` entre duas matrizes.
matriz_t *produto_matrizes_seq(const matriz_t* a, const matriz_t* b);

//< Calcula paralelamente o produto `a * b` entre duas matrizes, dado o número de threads.
matriz_t *produto_matrizes_par(const matriz_t* a, const matriz_t* b, int num_threads);

#endif // MATRIX_PRODUCT_H
