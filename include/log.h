#ifndef LOG_H
#define LOG_H

#define ANSII_VERDE "\033[1;32m"
#define ANSII_BRANCO "\033[1m"
#define ANSII_AMARELO "\033[1;33m"
#define ANSII_VERMELHO "\033[1;31m"
#define ANSII_AZUL "\033[1;34m"
#define ANSII_CIANO "\033[1;36m"
#define ANSII_MAGENTA "\033[1;35m"
#define ANSII_RESET "\033[0m"

#define VERDE(texto) ANSII_VERDE texto ANSII_RESET
#define BRANCO(texto) ANSII_BRANCO texto ANSII_RESET
#define AMARELO(texto) ANSII_AMARELO texto ANSII_RESET
#define VERMELHO(texto) ANSII_VERMELHO texto ANSII_RESET
#define AZUL(texto) ANSII_AZUL texto ANSII_RESET
#define CIANO(texto) ANSII_CIANO texto ANSII_RESET
#define MAGENTA(texto) ANSII_MAGENTA texto ANSII_RESET

#endif