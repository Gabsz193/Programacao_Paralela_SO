#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Generates a vector of size n with random integers between min and max
int* generate_vector(size_t n, int min, int max);

// Prints the first k elements of the vector (for debugging)
void print_vector(const int* v, size_t n, size_t k);

// Returns the current wall-clock time in seconds
double current_time_seconds();

#endif // UTILS_H
