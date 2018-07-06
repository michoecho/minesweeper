//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "minesweeper.h"

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
SDL_Texture* gTileTexture = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

int tiles_x = 30;
int tiles_y = 16;
int tile_size = 24;

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
	SDL_CreateWindowAndRenderer(tile_size * tiles_x, tile_size * tiles_y,
			SDL_WINDOW_SHOWN, &gWindow, &gRenderer);
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
	char *tiles = "res/tiles.png";
	gTileTexture = loadTexture(tiles);

	if (gTileTexture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
				"Unable to load image %s! SDL Error: %s\n", tiles, SDL_GetError());
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

enum crops {
	HIDDEN = 0,
	GOOD_FLAG = 1,
	BAD_FLAG = 2,
	GOOD_MINE = 3,
	BAD_MINE = 4,
	ZERO = 5,
};

int chooseSprite (Tile *targetTile, bool gameOver) {
	int sprite;
	if (!gameOver) {
		if (targetTile->state == FLAGGED) {
			sprite = GOOD_FLAG;
		} else if (targetTile->state == COVERED) {
			sprite = HIDDEN;
		} else {
			sprite = ZERO + targetTile->minedNeighbours;
		}
	} else {
		if (targetTile->state == FLAGGED) {
			sprite = targetTile->mined ? GOOD_FLAG : BAD_FLAG;
		} else if (targetTile->state == COVERED) {
			sprite = targetTile->mined ? GOOD_MINE : HIDDEN;
		} else {
			sprite = targetTile->mined ? BAD_MINE : ZERO + targetTile->minedNeighbours;
		}
	}
	return sprite;
}


void renderBoard(Board *b) {
	for (int x = 0; x < b->width; ++x)
	for (int y = 0; y < b->height; ++y) {
		Tile *targetTile = getTile(b, x, y);
		int sprite = chooseSprite(targetTile, b->state != RUNNING);
		SDL_Rect spriteRect = {64 * sprite, 0, 64, 64};
		SDL_Rect dstRect = {tile_size * x, tile_size * y, tile_size, tile_size};
		SDL_RenderCopy(gRenderer, gTileTexture, &spriteRect, &dstRect);
	}
}

void renderHighlight() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	x = x / tile_size;
	y = y / tile_size;
	SDL_Rect rect = {x * tile_size, y * tile_size, tile_size, tile_size};
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_ADD);
	SDL_RenderDrawRect(gRenderer, &rect);
}

bool isNumber(char *string) {
	if (!string || !*string) return false;
	while (*string) {
		if (!isdigit(*string)) return false;
		++string;
	}
	return true;
}

int main(int argc, char* args[])
{
	if (argc != 5) {
		fprintf(stdout, "Usage: minesweeper tiles_x tiles_y mine_count tile_size\n");
		return 1;
	}
	for (int i = 1; i <= 4; ++i) {
		if (!isNumber(args[i])) {
			fprintf(stdout, "Usage: minesweeper tiles_x tiles_y mine_count tile_size\n");
			return 1;
		}
	}

	tiles_x = strtol(args[1], NULL, 10);
	tiles_y = strtol(args[2], NULL, 10);
	int mine_count = strtol(args[3], NULL, 10);
	tile_size = strtol(args[4], NULL, 10);

	if (mine_count > tiles_x * tiles_y || tiles_x <= 0 || tiles_y <= 0 || tile_size <= 0) {
		fprintf(stdout, "Invalid options.");
		return 1;
	}

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

	Board *board = makeBoard(tiles_x,tiles_y);
	populateBoard(board, mine_count, time(NULL));

	//Main loop
	while (!quit) {
		//Event loop
		for (SDL_Event e; SDL_PollEvent(&e) != 0; ) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_q:
					quit = true;
					break;
				case SDLK_r:
					freeBoard(board);
					board = makeBoard(tiles_x, tiles_y);
					populateBoard(board, mine_count, time(NULL));
				}
			} else if (e.type == SDL_MOUSEBUTTONUP && board->state == RUNNING) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					uncoverTile(board, e.button.x / tile_size, e.button.y / tile_size);
				} else if (e.button.button == SDL_BUTTON_RIGHT) {
					flagTile(board, e.button.x / tile_size, e.button.y / tile_size);
				}
			}
		}

		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear(gRenderer);

		//Render texture to screen
		renderBoard(board);
		renderHighlight();

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

finish:
	freeBoard(board);
	//Free resources and close SDL
	finish();

	return 0;
}
