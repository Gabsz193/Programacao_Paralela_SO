#ifndef SCALAR_PRODUCT_H
#define SCALAR_PRODUCT_H

#include <stddef.h>

// Sequential scalar product
long long scalar_product_seq(const int* v1, const int* v2, size_t n);

// Parallel scalar product using pthreads
long long scalar_product_par(const int* v1, const int* v2, size_t n, int num_threads);

#endif // SCALAR_PRODUCT_H
