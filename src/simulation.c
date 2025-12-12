#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
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
    WINDOW_WIDTH = 240,
    WINDOW_HEIGHT = 120,
    RAIO_MIN = 5,
    RAIO_MAX = 30,
    VEL_MAX = 5,
};

typedef struct {
    float x;
    float y;
    float r;
    float vx;
    float vy;
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
static SDL_Renderer *renderer = NULL;
static SDL_Surface *canvas = NULL;

//< Os círculos gerados pelo programa.
static Circulo* circulos;
static size_t n_circulos;

//< Variáveis usadas para calcular o FPS.
static Uint64 fpsPerf = 0;
static int frames = 0;

//< Inicializa os círculos com valores aleatórios.
int inicializar_circulos(Args *args) {
    // Alocar os círculos
    n_circulos = args->size;
    circulos = calloc(n_circulos, sizeof(Circulo));

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
    return (dcx * dcx) + (dcy * dcy) <= (dr * dr);
}

//< Move todos os círculos e previne colisões entre as bordas.
void mover_circulos() {
    for (int i = 0; i < n_circulos; i++) {
        Circulo *c = circulos + i;
        c->x += c->vx;
        c->y += c->vy;

        if (c->x <= c->r) { c->x = c->r; c->vx = -c->vx; }
        if (c->y <= c->r) { c->y = c->r; c->vy = -c->vy; }
        if (c->x >= canvas->w - c->r) { c->x = canvas->w - c->r; c->vx = -c->vx; }
        if (c->y >= canvas->h - c->r) { c->y = canvas->h - c->r; c->vy = -c->vy; }
    }
}

//< Trata colisões entre os círculos.
void tratar_colisoes() {
    for (int i = 0; i < n_circulos; i++) {
        for (int j = i; j < n_circulos; j++) {
            Circulo *a = circulos + i;
            Circulo *b = circulos + j;

            // Verificar se os círculos colidem
            if (!circulos_colidem(a, b)) continue;

            // a->vx = 0;
            // a->vy = 0;
            // b->vx = 0;
            // b->vy = 0;
        }
    }
}

//< Renderiza uma seção da tela.
void renderizar(const SDL_Rect* rect) {
    SDL_Rect defaultRect = { .x = 0, .y = 0, .w = 640, .h = 480 };

    // Usar um rect padrão para a tela toda.
    if (rect == NULL)
        rect = &defaultRect;

    for (int y = rect->y; y < rect->y + rect->h; y++) {
        for (int x = rect->x; x < rect->x + rect->w; x++) {
            // Inicializar cor como branca
            Uint32 color = SDL_MapSurfaceRGBA(canvas, 255, 255, 255, 255);

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
int inicializar_threads(Args *args) {
    threads = calloc(args->threads, sizeof(ThreadInfo));
    if (threads == NULL) {
        SDL_Log("Não foi possível alocar memória para os dados das threads.");
        return 0;
    }
    
    if (pthread_barrier_init(&rendInicio, NULL, args->threads + 1) != 0) {
        SDL_Log("Não foi possível inicializar as barreiras: %s", strerror(errno));
        return 0;
    };

    if (pthread_barrier_init(&rendFim, NULL, args->threads + 1) != 0) {
        SDL_Log("Não foi possível inicializar as barreiras: %s", strerror(errno));
        return 0;
    };
    
    int tamanho_segmento = WINDOW_HEIGHT / args->threads;
    int resto = WINDOW_HEIGHT % args->threads;
    int inicio_segmento = 0;

    for (int i = 0; i < args->threads; i++) {
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

//< Executa do início do programa,
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    Args args = validar_argumentos(argc, argv);
    SDL_srand(args.seed);

    if (!inicializar_circulos(&args))
        return SDL_APP_FAILURE;

    if (!inicializar_threads(&args))
        return SDL_APP_FAILURE;
    
    // Inicializar SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Não foi possível inicializar o SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Criar renderizador da tela
    if (!SDL_CreateWindowAndRenderer("Simulador", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Não foi possível criar janela/renderizador: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    canvas = SDL_CreateSurface(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_PIXELFORMAT_RGBA8888);

    return SDL_APP_CONTINUE;
}

//< Executa quando receber um evento (mouse, teclado, ...).
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

//< Executa a cada tick do programa contínuamente.
SDL_AppResult SDL_AppIterate(void *appstate) {
    Uint64 perf = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();
    
    // Tratar colisões entre círculos círculos
    mover_circulos();
    
    // Desenhar quadro renderizado manualmente na janela
    pthread_barrier_wait(&rendInicio);
    pthread_barrier_wait(&rendFim);

    SDL_BlitSurface(canvas, 0, SDL_GetWindowSurface(window), 0);
    
    // Enviar quadro novo para a tela
    SDL_UpdateWindowSurface(window);
    
    // Atualizar FPS
    frames++;
    if (perf - fpsPerf >= freq) {
        double fps = frames;
        fps /= (double)(perf - fpsPerf) / (double)freq;

        // Imprimir FPS
        printf("FPS: %.2lf\n", fps);
        fflush(stdout);

        fpsPerf = perf;
        frames = 0;
    }

    return SDL_APP_CONTINUE;
}

//< Executa no fim da aplicação.
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}