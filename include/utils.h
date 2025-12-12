#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/**
 * @brief Representa os argumentos passados para os programas que calculam
 * produto interno de vetores e produto de matrizes.
 */
typedef struct {
    //< O tamanho dos vetores/matrizes quadradas.
    size_t size;

    //< A quantidade de threads a serem usadas.
    int threads;

    //< O modo de execução (sequencial ou paralelo)
    enum { SEQ, PAR } mode;

    //< A seed aleatória passada por argumento.
    unsigned int seed;
} Args;

/**
 * @brief Valida e obtem os argumentos passados por linha de comando.
 * @param argc A contagem de argumentos passados.
 * @param argv Um vetor de strings contendo os argumentos separados por
 * espaços em branco.
 * @returns Uma lista de argumentos lidos do `argv`. Caso não sejam passados
 * argumentos o suficiente, ou caso os argumentos sejam inválidos, a função
 * aborta a execução do programa.
 */
Args validar_argumentos(int argc, char* argv[]);

//< Retorna o tempo atual em segundos.
double tempo_segundos();

#endif // UTILS_H
