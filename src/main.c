//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "minesweeper.h"
#include "stopwatch.h"

bool init();
bool loadMedia();
void finish();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTileTexture = NULL;
SDL_Texture* gScreen = NULL;
char *gResPath = NULL;
TTF_Font *gFont = NULL;

int tiles_x = 30;
int tiles_y = 16;
int uiFontSize = 16;
int uiBarSize = 24;
int tile_size = 24;
SDL_Rect boardViewport;
SDL_Rect uiViewport;

SDL_Color RED = {0xFF, 0x00, 0x00};
SDL_Color GREEN = {0x03, 0x68, 0x0e};
SDL_Color BLACK = {0x00, 0x00, 0x00};
SDL_Color WHITE = {0xFF, 0xFF, 0xFF};


int floorDiv(int a, int b) { return a / b - (a < 0); }

bool init()
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

	gResPath = SDL_GetBasePath();
	if (!gResPath) gResPath = SDL_strdup("./");
	
	gWindow = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			boardViewport.w, boardViewport.h + uiViewport.h, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				"Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	gScreen = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 
		boardViewport.w, boardViewport.h);

	SDL_SetWindowTitle(gWindow, "Minesweeper");
	
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
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Unable to load image %s! SDL Error: %s\n", tiles, SDL_GetError());
		return false;
	}
	
	gFont = TTF_OpenFont( "res/font.ttf", uiFontSize);
	if( gFont == NULL )
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
		return false;
	}
	return true;
}

void finish()
{
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	gTileTexture = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_free(gResPath);

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
	SDL_SetRenderTarget(gRenderer, gScreen);
	for (int x = 0; x < b->width; ++x)
	for (int y = 0; y < b->height; ++y) {
		Tile *targetTile = getTile(b, x, y);
		int sprite = chooseSprite(targetTile, b->state != RUNNING);
		SDL_Rect spriteRect = {64 * sprite, 0, 64, 64};
		SDL_Rect dstRect = {tile_size * x, tile_size * y, tile_size, tile_size};
		SDL_RenderCopy(gRenderer, gTileTexture, &spriteRect, &dstRect);
	}
	SDL_SetRenderTarget(gRenderer, NULL);
}

void displayBoard(Board *b) {
	SDL_RenderCopy(gRenderer, gScreen, NULL, NULL);
}

void renderHighlight() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	int tile_x = floorDiv(x - boardViewport.x, tile_size);
	int tile_y = floorDiv(y - boardViewport.y, tile_size);
	SDL_Rect rect = {tile_x * tile_size, tile_y * tile_size, tile_size, tile_size};
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0x7F );
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(gRenderer, &rect);
}

void renderTime() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	x = x / tile_size;
	y = y / tile_size;
	SDL_Rect rect = {x * tile_size, y * tile_size, tile_size, tile_size};
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0x7F );
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(gRenderer, &rect);
}

bool isNumber(char *string) {
	if (!string || !*string) return false;
	while (*string) {
		if (!isdigit(*string)) return false;
		++string;
	}
	return true;
}

SDL_Texture * renderText(char *text, SDL_Color color) {
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended( gFont, text, color);
	if( textSurface == NULL ) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
		return NULL;
	}
		//Create texture from surface pixels
        SDL_Texture * texture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
	if( texture == NULL ) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
	}

	SDL_FreeSurface(textSurface);
	
	return texture;
}

void displayTime(unsigned time, SDL_Color color) {
	char timeText[100];
	sprintf(timeText, "%d:%02d.%03d", time/60000, (time % 60000) / 1000, time % 1000);
	SDL_Texture* timeTexture = renderText(timeText, color);
	SDL_Rect timeField = {20, 0, 0, 0};
	SDL_QueryTexture(timeTexture, NULL, NULL, &timeField.w, &timeField.h);
	timeField.y = (uiViewport.h - timeField.h) / 2;
	SDL_RenderCopy(gRenderer, timeTexture, NULL, &timeField);
	SDL_DestroyTexture(timeTexture);
}	

void displayFlagCount(Board *b) {
	char text[100];
	sprintf(text, "%d/%d", b->flagCount, b->mineCount);
	SDL_Texture* texture = renderText(text, BLACK);
	SDL_Rect field;
	SDL_QueryTexture(texture, NULL, NULL, &field.w, &field.h);
	field.x = uiViewport.w - field.w - 20;
	field.y = (uiViewport.h - field.h) / 2;
	SDL_RenderCopy(gRenderer, texture, NULL, &field);
	SDL_DestroyTexture(texture);
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
	boardViewport = (SDL_Rect){0, uiBarSize, tiles_x * tile_size, tiles_y * tile_size};
	uiViewport = (SDL_Rect){0, 0, tiles_x * tile_size, uiBarSize};

	if (mine_count > tiles_x * tiles_y || tiles_x <= 0 || tiles_y <= 0 || tile_size <= 0) {
		fprintf(stdout, "Invalid options.");
		return 1;
	}

	Board *board = NULL;

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

	board = makeBoard(tiles_x, tiles_y, mine_count);
	resetBoard(board, time(NULL));
	renderBoard(board);

	Stopwatch *sw = makeStopwatch();

	bool paused = false;

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
					resetBoard(board, time(NULL));
					restartStopwatch(sw);
					paused = false;
					renderBoard(board);
					break;
				case SDLK_p:
					if (paused) {
						unpauseStopwatch(sw);
						paused = false;
					} else {
						pauseStopwatch(sw);
						paused = true;
					}
				}
			} else if (e.type == SDL_MOUSEBUTTONUP && board->state == RUNNING) {
				int tile_x = floorDiv(e.button.x - boardViewport.x, tile_size);
				int tile_y = floorDiv(e.button.y - boardViewport.y, tile_size);
				if (e.button.button == SDL_BUTTON_LEFT) {
					uncoverTile(board, tile_x, tile_y);
					renderBoard(board);
					unpauseStopwatch(sw);
				} else if (e.button.button == SDL_BUTTON_RIGHT) {
					flagTile(board, tile_x, tile_y);
					renderBoard(board);
					unpauseStopwatch(sw);
				}
			}
		}

		//Clear screen
		SDL_RenderSetViewport(gRenderer, NULL);
		if (board->state == RUNNING)
			SDL_SetRenderDrawColor(gRenderer, 0xb5, 0xb5, 0xb5, 0xFF);
		else if (board->state == WON)
			SDL_SetRenderDrawColor(gRenderer, GREEN.r, GREEN.g, GREEN.b, 0xFF);
		else if (board->state == LOST)
			SDL_SetRenderDrawColor(gRenderer, RED.r, RED.g, RED.b, 0xFF);
		SDL_RenderClear(gRenderer);

		if (board->state != RUNNING)
			pauseStopwatch(sw);

		//Render texture to screen
		if (!paused) {
			SDL_RenderSetViewport(gRenderer, &boardViewport);
			displayBoard(board);
			renderHighlight();

			SDL_RenderSetViewport(gRenderer, &uiViewport);
			displayTime(readStopwatch(sw), BLACK);
			displayFlagCount(board);
		}

		//Update screen
		SDL_RenderPresent(gRenderer);
		SDL_Delay(16);
	}

finish:
	freeBoard(board);
	finish();

	return 0;
}
