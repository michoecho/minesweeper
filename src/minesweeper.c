#include <stdbool.h>
#include <stdlib.h>
#include "minesweeper.h"

Board *
makeBoard(int width, int height)
{
	Board *board = malloc(sizeof(Board));
	board->width = width;
	board->height = height;
	board->state = RUNNING;
	board->tiles = malloc(width * sizeof(Tile*));
	for (int x = 0; x < width; ++x) {
		board->tiles[x] = malloc(height * sizeof(Tile));
		for (int y = 0; y < height; ++y)
			board->tiles[x][y] = (Tile){COVERED, false};
	}
	return board;
}

void freeBoard(Board *board) {
	for (int i = 0; i < board->width; ++i) {
		free(board->tiles[i]);
	}
	free(board->tiles);
	free(board);
}

bool inBoard(Board *board, int x, int y)
{
	return x >= 0 && x < board->width && y >=0 && y < board->height;
}

Tile *
getTile(Board *board, int x, int y)
{
	if (!(x >= 0 && x < board->width && y >=0 && y < board->height)) return NULL;
	return &(board->tiles[x][y]);
}

void populateBoard(Board *board, int numOfMines, unsigned seed)
{
	srand(seed);
	while (numOfMines > 0) {
		int x = rand() % board->width;
		int y = rand() % board->height;
		Tile *targetTile = getTile(board, x, y);	
		if (!targetTile->mined) {
			targetTile->mined = true;
			numOfMines -= 1;
		}	
	}
	board->remainingTiles = board->height * board->width - numOfMines;
	for (int x = 0; x < board->width; ++x)
	for (int y = 0; y < board->height; ++y)
		getTile(board, x, y)->minedNeighbours = minedNeighbours(board, x, y);
}


int minedNeighbours(Board *board, int x, int y)
{
	int cnt = 0;
	for (int dx = -1; dx <= 1; ++dx)
	for (int dy = -1; dy <= 1; ++dy) {
		if (dx == 0 && dy == 0) continue;
		Tile *targetTile = getTile(board, x+dx, y+dy);
		if (targetTile && targetTile->mined)
			++cnt;
	}
	return cnt;
}

void flagTile(Board *board, int x, int y)
{
	Tile *targetTile = getTile(board, x, y);
	if (!targetTile) return;
	if (targetTile->state == UNCOVERED) return;

	if (targetTile->state == FLAGGED) {
		targetTile->state = COVERED;
		board->remainingTiles += 1;
	} else {
		targetTile->state = FLAGGED;
		board->remainingTiles -= 1;
	}
}	

void uncoverTile(Board *board, int x, int y)
{
	Tile *targetTile = getTile(board, x, y);
	if (!targetTile) return;

	if (targetTile->state != COVERED) return;
	targetTile->state = UNCOVERED;

	if (targetTile->mined) {
		board->state = LOST;
		return;
	}

	board->remainingTiles -= 1;

	if (board->remainingTiles <= 0) {
		board->state = WON;
		return;
	}

	if (targetTile->minedNeighbours == 0) {
		for (int dx = -1; dx <= 1; ++dx)
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			uncoverTile(board, x + dx, y + dy);
		}
	}
}	
