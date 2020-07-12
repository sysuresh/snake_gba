#ifndef GAME_H
#define GAME_H

#include "gba.h"
#define GAME_WIDTH WIDTH
#define SCALE 10
#define GAME_HEIGHT ((HEIGHT) - 40)
#define ORIGINAL_LENGTH 3
#define ORIGINAL_DELAY 40

typedef struct cell {
  int dr, dc, r, c;
} Cell;

#endif
