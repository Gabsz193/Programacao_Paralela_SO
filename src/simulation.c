#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "pthread.h"
#include "utils.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

//< Constantes de execução do programa
enum {
    //< Tempo de execução do programa (s)
    TEMPO_EXECUCAO = 3,
    //< Comprimento da janela (px)
    WINDOW_WIDTH = 640,
    //< Altura da janela (px)
    WINDOW_HEIGHT = 480,
    //< Raio mínimo dos círculos (px)
    RAIO_MIN = 10,
    //< Raio máximo dos círculos (px)
    RAIO_MAX = 30,
    //< Velocidade inicial máxima dos círculos (px)
    VEL_MAX = 5,
};

typedef struct {
    double r;
    double x, y;
    double vx, vy;
    Uint32 c;
} Circulo;

typedef struct {
    //< A thread que utiliza esta struct.
    pthread_t thread;

    //< O retângulo que esta thread irá renderizar.
    SDL_Rect rect;
} ThreadInfo;

//< Variáveis utilizadas para gerenciar a janela e renderização.
static SDL_Window *window = NULL;
static SDL_Surface *canvas = NULL;

//< Os círculos gerados pelo programa.
static Circulo* circulos;
static size_t n_circulos;

//< Variáveis usadas para calcular o FPS.
static Uint64 fpsPerf = 0;
static int frames = 0;

//< Inicializa os círculos com valores aleatórios.
int inicializar_circulos(size_t qtd) {
    // Alocar os círculos
    n_circulos = qtd;
    circulos = malloc(n_circulos * sizeof(Circulo));

    if (!circulos) {
        SDL_Log("Não foi possível alocar os círculos.");
        return 0;
    }

    // Criar os círculos
    for (size_t i = 0; i < n_circulos; i++) {
        Circulo* c = circulos + i;
        
        // Gerar cor opaca aleatória
        c->c = SDL_rand_bits() | 0xFF;

        // Gerar raio aleatório entre [RAIO_MIN, RAIO_MAX)
        c->r = RAIO_MIN + SDL_randf() * (RAIO_MAX - RAIO_MIN);
        
        // Gerar posição aleatória dentro da tela
        c->x = c->r + (SDL_randf() * (WINDOW_WIDTH  - (2 * c->r)));
        c->y = c->r + (SDL_randf() * (WINDOW_HEIGHT - (2 * c->r)));

        // Gerar velocidade entre [-VEL_MAX, VEL_MAX)
        c->vx = (SDL_randf() - 0.5f) * 2 * VEL_MAX;
        c->vy = (SDL_randf() - 0.5f) * 2 * VEL_MAX;
    }

    return 1;
}

//< Retorna o resultado da colisão entre dois círculos.
int circulos_colidem(const Circulo *a, const Circulo *b) {
    float dcx = a->x - b->x;
    float dcy = a->y - b->y;
    float dr = a->r + b->r;
    return ((dcx * dcx) + (dcy * dcy) < (dr * dr));
}

//< Retorna `1` se for possível realizar o movimento de `c` para `(xd, yd)`
int tentar_movimento(Circulo *a, double dx, double dy) {
    double ox = a->x;
    double oy = a->y;
    a->x = dx;
    a->y = dy;

    bool colidiu = true;
    size_t iter = 0;
    while (colidiu && iter++ < 4) {
        colidiu = false;
        for (int i = 0; i < n_circulos; i++) {
            Circulo *b = circulos + i;
            
            if (a == b) continue;

            if (circulos_colidem(a, b)) {
                colidiu = true;

                /*
                    Se A se mover para D(dx, dy), os círculos A e B colidem.
                    Para evitar essa colisão, calcula-se o parâmetro `p1`, usado para
                    calcular uma nova posição D', dado que...
                        D' = A + p (D - A)
                    tal que...
                        || D' - B || = rA + rB
                */
                double ux = dx - ox;
                double uy = dy - oy;
                double vx = b->x - ox;
                double vy = b->y - oy;
                double t = vx * ux + vy * uy;
                double R2 = SDL_pow(a->r + b->r, 2);
                double h2 = (vx * vx + vy * vy) - (t * t);
                
                if (h2 > R2) h2 = R2;
                double p = t - SDL_sqrt(R2 - h2);

                a->x = dx = ox + p * ux;
                a->y = dy = oy + p * uy;


                double ma = SDL_sqrt(a->r);
                double mb = SDL_sqrt(b->r);

                //< Igual a A - B
                double xAdB = a->x - b->x, yAdB = a->y - b->y;

                //< Igual a ||A - B||
                double lAdB = SDL_sqrt((xAdB * xAdB) + (yAdB * yAdB));
                
                //< Igual a (A - B) / ||A - B||
                double xNhat = xAdB / lAdB, yNhat = yAdB / lAdB;
                
                //< Igual a Va - Vb
                double xVadVb = a->vx - b->vx, yVadVb = a->vy - b->vy;

                //< Igual a (Va - Vb) * nhat
                double Vn = (xVadVb * xNhat) + (yVadVb * yNhat);
                double j = (- (1.0 + SDL_randf()) * Vn) / ( (1./ma) + (1./mb) );
                
                //< Igual a Vn * nhat
                a->vx -= xNhat * Vn;
                a->vy -= yNhat * Vn;
                b->vx += xNhat * Vn;
                b->vy += yNhat * Vn;
            }
        }
    }

    return 1;
}

//< Move todos os círculos e previne colisões entre as bordas.
void mover_circulos() {
    for (int i = 0; i < n_circulos; i++) {
        Circulo *c = circulos + i;
        int flipx = 0;
        int flipy = 0;
        
        double dx = c->x + c->vx;
        double dy = c->y + c->vy;

        if (dx <= c->r) { dx = c->r; flipx = 1; }
        if (dy <= c->r) { dy = c->r; flipy = 1; }
        if (dx >= canvas->w - c->r) { dx = canvas->w - c->r; flipx = 1; }
        if (dy >= canvas->h - c->r) { dy = canvas->h - c->r; flipy = 1; }

        tentar_movimento(c, dx, dy);

        if (flipx) c->vx = -c->vx;
        if (flipy) c->vy = -c->vy;
    }
}

//< Renderiza uma seção da tela.
void renderizar(const SDL_Rect* rect) {
    SDL_Rect defaultRect = { .x = 0, .y = 0, .w = WINDOW_WIDTH, .h = WINDOW_HEIGHT };

    // Usar um rect padrão para a tela toda.
    if (rect == NULL)
        rect = &defaultRect;

    for (int y = rect->y; y < rect->y + rect->h; y++) {
        for (int x = rect->x; x < rect->x + rect->w; x++) {
            // Inicializar cor como branca
            Uint32 color = 0xFFFFFFFF;

            // Desenhar todos os círculos
            for (int i = 0; i < n_circulos; i++) {
                int xc = x - circulos[i].x;
                int yc = y - circulos[i].y;
                int r = circulos[i].r;

                // Verificar se o píxel está dentro do círculo
                if (sqrtf(xc * xc + yc * yc) < r) {
                    color = circulos[i].c;
                }
            }

            // Achar ponteiro do pixel
            int bpp = SDL_BYTESPERPIXEL(canvas->format);
            Uint32 *pixel =  canvas->pixels + y * canvas->pitch + x * bpp;

            *pixel = color;
        }
    }
}

//< Barreiras de sincronização entre as threads.
static pthread_barrier_t rendInicio;
static pthread_barrier_t rendFim;
static ThreadInfo* threads;

//< A função principal de cada thread.
void *render_thread_main(void *_info) {
    ThreadInfo *info = _info;

    // Contínuamente renderiza o mesmo retângulo
    while (true) {
        pthread_barrier_wait(&rendInicio);
        renderizar(&info->rect);
        pthread_barrier_wait(&rendFim);
    }

    return NULL;
}

//< Inicializa todas as threads do programa.
int inicializar_threads(size_t n_threads) {
    threads = calloc(n_threads, sizeof(ThreadInfo));
    if (threads == NULL) {
        SDL_Log("Não foi possível alocar memória para os dados das threads.");
        return 0;
    }
    
    if (pthread_barrier_init(&rendInicio, NULL, n_threads + 1) != 0) {
        SDL_Log("Não foi possível inicializar as barreiras: %s", strerror(errno));
        return 0;
    };

    if (pthread_barrier_init(&rendFim, NULL, n_threads + 1) != 0) {
        SDL_Log("Não foi possível inicializar as barreiras: %s", strerror(errno));
        return 0;
    };
    
    int tamanho_segmento = WINDOW_HEIGHT / n_threads;
    int resto = WINDOW_HEIGHT % n_threads;
    int inicio_segmento = 0;

    for (int i = 0; i < n_threads; i++) {
        ThreadInfo *t = threads + i;
        
        t->rect = (SDL_Rect) {
            .w = WINDOW_WIDTH,
            .h = tamanho_segmento + (i < resto),
            .x = 0,
            .y = inicio_segmento,
        };

        inicio_segmento += t->rect.h;

        // Criar as threads
        if (pthread_create(&t->thread, NULL, render_thread_main, t) != 0) {
            SDL_Log("Não foi possível inicializar as threads: %s", strerror(errno));
            return 0;
        }
    }

    return 1;
}

//< Renderizador sequencial - renderiza cada frame sequencialmente
void renderizador_seq(void) {
    renderizar(NULL);
}

//< Renderizador paralelo - aguarda cada uma das threads 
void renderizador_par(void) {
    pthread_barrier_wait(&rendInicio);
    pthread_barrier_wait(&rendFim);
}

//< Usado para se referir genericamente à `renderizador_seq` ou `renderizador_par`
typedef void(*renderizador_f)(void);

//< Executa do início do programa,
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    renderizador_f* renderizador = (renderizador_f*)appstate;
    Args args = validar_argumentos(argc, argv);
    SDL_srand(args.seed);

    if (!inicializar_circulos(args.size))
        return SDL_APP_FAILURE;

    // Inicializar estado do renderizador
    if (args.mode == PAR) {
        *renderizador = renderizador_par;

        if (!inicializar_threads(args.threads))
            return SDL_APP_FAILURE;
    } else {
        *renderizador = renderizador_seq;
    }
    
    // Inicializar SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Não foi possível inicializar o SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Criar renderizador da tela
    if (!(window = SDL_CreateWindow("Simulador", WINDOW_WIDTH, WINDOW_HEIGHT, 0))) {
        SDL_Log("Não foi possível criar janela/renderizador: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Imprimir parte da saída final do programa
    printf("%s,%zu,%d,", args.mode == SEQ ? "seq" : "par", args.size, args.threads);

    canvas = SDL_CreateSurface(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_PIXELFORMAT_RGBA8888);
    fpsPerf = SDL_GetPerformanceCounter();
    return SDL_APP_CONTINUE;
}

//< Executa quando receber um evento (mouse, teclado, ...).
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

//< Executa a cada tick do programa contínuamente.
SDL_AppResult SDL_AppIterate(void *appstate) {
    renderizador_f renderizador = (renderizador_f)appstate;
    Uint64 perf = SDL_GetPerformanceCounter();
    Uint64 freq = (Uint64)TEMPO_EXECUCAO * SDL_GetPerformanceFrequency();
    
    // Tratar colisões entre círculos círculos
    mover_circulos();

    // Desenhar quadro renderizado manualmente na janela
    renderizador();

    SDL_BlitSurface(canvas, 0, SDL_GetWindowSurface(window), 0);
    
    // Enviar quadro novo para a tela
    SDL_UpdateWindowSurface(window);
    
    // Atualizar FPS
    frames++;
    
    // Imprimir FPS após o período determinado e finalizar a execução
    if (perf - fpsPerf >= freq) {
        double fps = frames;
        fps /= (double)(perf - fpsPerf) / (double)freq;

        // Imprimir FPS e finalizar a execução
        printf("%.6f\n", fps);
        fflush(stdout);
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

//< Executa no fim da aplicação.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}