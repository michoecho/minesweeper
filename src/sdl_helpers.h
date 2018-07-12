#ifndef SDL_HELPERS_H_
#define SDL_HELPERS_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>

typedef struct Context_SDL {
	SDL_Window *window;
	SDL_Renderer *renderer;
} Context_SDL;

//bool init_SDL(Context_SDL* ctx);

SDL_Texture* loadTextureFromMemory(void *beg, int size, SDL_Renderer *renderer);
SDL_Surface* loadSurfaceFromMemory(void *ptr, int size);
TTF_Font* loadFontFromMemory(void *ptr, int size, int fontSize);
SDL_Surface* loadSurface(char* relPath);
SDL_Texture* loadTexture(char* relPath, SDL_Renderer *renderer);
SDL_Texture* renderText(char *text, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);

#endif
