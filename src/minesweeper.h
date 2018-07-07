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
	int coveredTiles;
	int flagCount;
	int mineCount;
	Tile *tiles;
} Board;

Board * makeBoard(int width, int height, int mineCount);

Tile * getTile(Board *board, int x, int y);

void freeBoard(Board *board); 

void resetBoard(Board *board, unsigned seed);

void uncoverTile(Board *board, int x, int y);

void flagTile(Board *board, int x, int y);
#endif
