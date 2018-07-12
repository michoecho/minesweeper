#include <stdbool.h>
#include <stdlib.h>
#include "minesweeper.h"

Tile *
getTile(Board *board, int x, int y)
{
	if (!(x >= 0 && x < board->width && y >=0 && y < board->height)) return NULL;
	return &(board->tiles[y * board->width + x]);
}

Board *
makeBoard(int width, int height, int mineCount)
{
	Board *board = malloc(sizeof(Board));
	board->width = width;
	board->height = height;
	board->mineCount = mineCount;
	board->tiles = malloc(width * height * sizeof(Tile));
	return board;
}

void freeBoard(Board *board) {
	if (!board) return;
	free(board->tiles);
	free(board);
}

void incrementNeighbours (Board *board, int x, int y) {
	for (int dx = -1; dx <= 1; ++dx)
	for (int dy = -1; dy <= 1; ++dy) {
		Tile *targetTile = getTile(board, x+dx, y+dy);
		if (targetTile)
			targetTile->minedNeighbours += 1;
	}
}

void resetBoard(Board *board, unsigned seed)
{
	board->state = RUNNING;
	board->flagCount = 0;
	board->coveredTiles = board->width * board->height;
	for (int x = 0; x < board->width; ++x)
	for (int y = 0; y < board->height; ++y) {
		*getTile(board, x, y) = (Tile){.state = COVERED, .mined = false, .minedNeighbours = 0};
	}
	srand(seed);
	int remainingMines = board->mineCount;
	while (remainingMines > 0) {
		int x = rand() % board->width;
		int y = rand() % board->height;
		Tile *targetTile = getTile(board, x, y);
		if (targetTile->mined) continue;
		targetTile->mined = true;
		remainingMines -= 1;
		incrementNeighbours(board, x, y);
	}
}

void flagTile(Board *board, int x, int y)
{
	Tile *targetTile = getTile(board, x, y);
	if (!targetTile) return;
	if (targetTile->state == UNCOVERED) return;

	if (targetTile->state == FLAGGED) {
		targetTile->state = COVERED;
		board->flagCount -= 1;
	} else {
		targetTile->state = FLAGGED;
		board->flagCount += 1;
	}
}


void uncoverTile(Board *board, int x, int y)
{
	Tile *targetTile = getTile(board, x, y);
	if (!targetTile) return;

	if (targetTile->state != COVERED) return;
	targetTile->state = UNCOVERED;
	board->coveredTiles -= 1;

	if (targetTile->mined) {
		board->state = LOST;
		return;
	}

	if (targetTile->minedNeighbours == 0) {
		for (int dx = -1; dx <= 1; ++dx)
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			uncoverTile(board, x + dx, y + dy);
		}
	}

	if (board->coveredTiles <= board->mineCount) {
		board->state = WON;
		for (int x = 0; x < board->width; ++x)
		for (int y = 0; y < board->height; ++y) {
			Tile *tile = getTile(board, x, y);
			if (tile->mined)
				tile->state = FLAGGED;
		}
	}
}
