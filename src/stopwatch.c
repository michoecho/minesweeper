#include "stopwatch.h"
#include <SDL.h>
#include <stdbool.h>

typedef struct Stopwatch {
	unsigned beg;
	unsigned acc;
	bool isPaused;
} Stopwatch;

Stopwatch *
makeStopwatch() {
	Stopwatch *sw = malloc(sizeof(Stopwatch));
	restartStopwatch(sw);
	return sw;
}

void freeStopwatch(Stopwatch *sw) {
	free(sw);
}

void restartStopwatch(Stopwatch *sw) {
	sw->acc = 0;
	sw->isPaused = true;
}

unsigned readStopwatch(Stopwatch *sw) {
	return sw->isPaused ? sw->acc : sw->acc + SDL_GetTicks() - sw->beg;
}

void pauseStopwatch(Stopwatch *sw) {
	if (sw->isPaused) return;
	sw->acc += SDL_GetTicks() - sw->beg;
	sw->isPaused = true;
}

void unpauseStopwatch(Stopwatch *sw) {
	if (!sw->isPaused) return;
	sw->beg = SDL_GetTicks();
	sw->isPaused = false;
}
