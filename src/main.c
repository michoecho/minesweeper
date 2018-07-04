//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;
	
char *gResPath = NULL;

bool init()
{
	//Initialize SDL
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

	gResPath = SDL_GetBasePath();
	if (!gResPath) gResPath = SDL_strdup("./");

	//Create window
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &gWindow, &gRenderer);
	if (gWindow == NULL || gRenderer == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Window or renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetWindowTitle(gWindow, "Minesweeper");
	
	//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	return true;
}

SDL_Surface* loadSurface(char* relPath)
{
	char path[4096];
	strcpy(path, gResPath);
	strcat(path, relPath);

	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
				"Unable to load image %s! SDL Error: %s\n", path, IMG_GetError());
	}

	return loadedSurface;
}

SDL_Texture* loadTexture(char* relPath)
{
	SDL_Surface* loadedSurface = loadSurface(relPath);
	SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if(loadedTexture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to create texture from %s! SDL Error: %s\n", relPath, SDL_GetError());
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);
	return loadedTexture;
}

bool loadMedia()
{
	char *helloWorld = "res/hello_world.bmp";
	gTexture = loadTexture(helloWorld);

	if (gTexture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
				"Unable to load image %s! SDL Error: %s\n", helloWorld, SDL_GetError());
		return false;
	}
	return true;
}

void finish()
{
	//Deallocate surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_free(gResPath);

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to initialize!\n");
		goto finish;
	}
	//Load media
	if (!loadMedia()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Failed to load media!\n");
		goto finish;
	}

	//Main loop flag
	bool quit = false;

	//Main loop
	while (!quit) {
		//Event loop
		for (SDL_Event e; SDL_PollEvent(&e) != 0; ) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym)
				{
					case SDLK_q: quit = true; break;
				}
			}
		}

		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear(gRenderer);

		//Render texture to screen
		SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

finish:
	//Free resources and close SDL
	finish();

	return 0;
}
