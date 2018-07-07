#ifndef STOPWATCH_H_
#define STOPWATCH_H_

typedef struct Stopwatch Stopwatch;
Stopwatch * makeStopwatch();
void freeStopwatch(Stopwatch *sw);
unsigned readStopwatch(Stopwatch *sw);
void restartStopwatch(Stopwatch *sw);
void pauseStopwatch(Stopwatch *sw);
void unpauseStopwatch(Stopwatch *sw);

#endif
