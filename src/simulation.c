#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct {
    int x;
    int y;
    int r;
    int vx;
    int vy;
    Uint32 c;
} Circulo;

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Surface *canvas = NULL;
static Circulo circulos[] = {
    { .x = 100, .y = 100, .r = 100, .vx = 2, .vy = 2, .c = 0xFF0000FF },
    { .x = 300, .y = 200, .r = 20, .vx = -3, .vy = 2, .c = 0xFF00FFFF },
    { .x = 400, .y = 90, .r = 30, .vx = 5, .vy = -1, .c = 0x00FF00FF },

};

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Simulador", "1.0", "br.edu.ufam.icomp.trabalho-so2");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Não foi possível inicializar o SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Simulador", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Não foi possível criar janela/renderizador: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    canvas = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_RGBA8888);

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Mover todos os círculos
    for (int i = 0; i < sizeof(circulos) / sizeof(Circulo); i++) {
        Circulo *c = circulos + i;
        c->x += c->vx;
        c->y += c->vy;

        if (c->x <= c->r) { c->x = c->r; c->vx = -c->vx; }
        if (c->y <= c->r) { c->y = c->r; c->vy = -c->vy; }
        if (c->x >= canvas->w - c->r) { c->x = canvas->w - c->r; c->vx = -c->vx; }
        if (c->y >= canvas->h - c->r) { c->y = canvas->h - c->r; c->vy = -c->vy; }
    }

    SDL_LockSurface(canvas);
    
    // Desenhar um fundo vermelho
    for (int y = 0; y < canvas->h; y++) {
        for (int x = 0; x < canvas->w; x++) {
            int xc = (x - canvas->w / 2);
            int yc = (y - canvas->h / 2);
            
            // Inicializar cor como branca
            Uint32 color = SDL_MapSurfaceRGBA(canvas, 255, 255, 255, 255);

            // Desenhar todos os círculos
            for (int i = 0; i < sizeof(circulos) / sizeof(Circulo); i++) {
                int xc = x - circulos[i].x;
                int yc = y - circulos[i].y;
                int r = circulos[i].r;

                // Verificar se o píxel está dentro do círculo
                if (xc * xc + yc * yc < r * r) {
                    color = circulos[i].c;
                }
            }

            // Achar ponteiro do pixel
            int bpp = SDL_BYTESPERPIXEL(canvas->format);
            Uint32 *pixel =  canvas->pixels + y * canvas->pitch + x * bpp;

            *pixel = color;
        }
    }
    
    SDL_UnlockSurface(canvas);
    
    // Desenhar quadro renderizado manualmente na janela
    SDL_BlitSurface(canvas, 0, SDL_GetWindowSurface(window), 0);
    
    // Enviar quadro novo para a tela
    SDL_UpdateWindowSurface(window);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}