#include "sdl_helpers.h"

char * basePath() {
	char *path = SDL_GetBasePath();
	return path ? path : "./";
}

bool init_SDL(Context_SDL *ctx)
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
				"Warning: Linear texture filtering not enabled!");
	}
	
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	 //Initialize SDL_ttf
	if( TTF_Init() == -1 )
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}


bool makeContext_SDL(Context_SDL *ctx)
{
	ctx->window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480, SDL_WINDOW_SHOWN);

	if (ctx->window == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (ctx->renderer == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

SDL_Surface* loadSurface(char* relPath)
{
	char path[4096];
	strcpy(path, basePath());
	strcat(path, relPath);

	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
				"Unable to load image %s! SDL Error: %s\n", path, IMG_GetError());
	}

	return loadedSurface;
}

SDL_Texture* loadTexture(char* relPath, SDL_Renderer *renderer)
{
	SDL_Surface* loadedSurface = loadSurface(relPath);
	SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if(loadedTexture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to create texture from %s! SDL Error: %s\n", relPath, SDL_GetError());
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);
	return loadedTexture;
}

SDL_Texture * renderText(char *text, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
	if( textSurface == NULL ) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
		return NULL;
	}
		//Create texture from surface pixels
        SDL_Texture * texture = SDL_CreateTextureFromSurface( renderer, textSurface );
	if( texture == NULL ) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
	}

	SDL_FreeSurface(textSurface);
	
	return texture;
}
