#ifndef SCALAR_PRODUCT_H
#define SCALAR_PRODUCT_H

#include <stddef.h>

//< Gera um vetor de tamanho `n` com números aleatórios entre `min` e `max`.
int* gerar_vetor(size_t n, int min, int max);

//< Calcula sequencialmente o produto escalar `v1 * v2` entre dois vetores.
long long produto_escalar_seq(const int* v1, const int* v2, size_t n);

//< Calcula paralelamente o produto escalar `v1 * v2` entre dois vetores, dado um número de threads.
long long produto_escalar_par(const int* v1, const int* v2, size_t n, int num_threads);

#endif // SCALAR_PRODUCT_H
