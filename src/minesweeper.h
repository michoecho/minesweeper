#ifndef MINESWEEPER_H_
#define MINESWEEPER_H_

enum TileState {
	UNCOVERED,
	COVERED,
	FLAGGED,
};

enum GameState {
	WON,
	LOST,
	RUNNING,
};

typedef struct Tile {
	enum TileState state;
	bool mined;
	int minedNeighbours;
} Tile;

typedef struct Board {
	enum GameState state;
	int width;
	int height;
	int remainingTiles;
	Tile** tiles;
} Board;

Board * makeBoard(int width, int height);

void freeBoard(Board *board); 

bool inBoard(Board *board, int x, int y);

Tile * getTile(Board *board, int x, int y);

void populateBoard(Board *board, int numOfMines, unsigned seed);

int minedNeighbours(Board *board, int x, int y);

void uncoverTile(Board *board, int x, int y);

void flagTile(Board *board, int x, int y);
#endif
