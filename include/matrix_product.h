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

/**
 * @brief Calcula sequencialmente o produto entre duas matrizes.
 * @param a A matriz à esquerda no produto.
 * @param b A matriz à direita no produto.
 * @returns A matriz resultante do produto `a * b`.
 */
matriz_t *produto_matrizes_seq(const matriz_t* a, const matriz_t* b);

/**
 * @brief Calcula paralelamente o produto escalar entre dois vetores.
 * @param a A matriz à esquerda no produto.
 * @param b A matriz à direita no produto.
 * @param num_threads O número de threads usadas para executar o produto entre matrizes.
 * @returns A matriz resultante do produto `a * b`.
 */
matriz_t *produto_matrizes_par(const matriz_t* a, const matriz_t* b, int num_threads);

#endif // MATRIX_PRODUCT_H
