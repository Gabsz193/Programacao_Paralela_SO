#ifndef SCALAR_PRODUCT_H
#define SCALAR_PRODUCT_H

#include <stddef.h>

/**
 * @brief Calcula sequencialmente o produto escalar entre dois vetores.
 * @param v1 O primeiro vetor do produto escalar.
 * @param v2 O segundo vetor do produto escalar.
 * @param n O tamanho dos vetores `v1` e `v2`.
 * @returns O resultado do produto escalar.
 */
long long produto_escalar_seq(const int* v1, const int* v2, size_t n);

/**
 * @brief Calcula paralelamente o produto escalar entre dois vetores.
 * @param v1 O primeiro vetor do produto escalar.
 * @param v2 O segundo vetor do produto escalar.
 * @param n O tamanho dos vetores `v1` e `v2`.
 * @param num_threads O número de threads usadas para executar o produto escalar.
 * @returns O resultado do produto escalar.
 */
long long produto_escalar_par(const int* v1, const int* v2, size_t n, int num_threads);

#endif // SCALAR_PRODUCT_H
