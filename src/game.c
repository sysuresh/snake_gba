#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "../images/start.h"
#include "../images/lose.h"
#include "../images/middle.h"
typedef enum {
  START,
  PLAY,
  WIN,
  LOSE,
} GBAState;

int grow(Cell *snake, int length)
{
  snake[length].r = snake[length - 1].r - snake[length - 1].dr;
  snake[length].c = snake[length - 1].c - snake[length - 1].dc;
  snake[length].dr = snake[length - 1].dr;
  snake[length].dc = snake[length - 1].dc;
  return length + 1;
}

void updateDirections(Cell *snake, int length)
{
  for (int i = length - 1; i > 0; i--)
  {
    snake[i].dr = snake[i - 1].dr;
    snake[i].dc = snake[i - 1].dc;
  }
}

void move(Cell *snake, int length) 
{
  for (int i = 0; i < length; i++)
  {
    snake[i].r = snake[i].r + snake[i].dr;
    snake[i].c = snake[i].c + snake[i].dc;
  }
}

int collision(Cell *snake, int length, int r, int c)
{
  for (int i = 0; i < length; i++)
    if (r == snake[i].r && c == snake[i].c)
      return 1;
  return 0;
}

void placeApple(Cell *snake, int length, int *appler, int *applec)
{
  do
  {
    *appler = randint(0, GAME_HEIGHT - 1) / SCALE;
    *applec = randint(0, GAME_WIDTH - 1) / SCALE;
  } while (collision(snake, length, *appler, *applec));
}

void updateScore(int length)
{
  char s[100];
  sprintf(s, "Score: %d", length - ORIGINAL_LENGTH);
  drawRectDMA(GAME_HEIGHT, 0, 60, HEIGHT - GAME_HEIGHT, BLACK);
  drawString(GAME_HEIGHT, 0, s, YELLOW);
}

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;
  u32 currentButtons = BUTTONS;
 
  GBAState state = START;

  int initialized = 0;
  Cell snake[GAME_HEIGHT*GAME_WIDTH / (SCALE * SCALE)];
  int length = 0; 
  int erase = 0, eraser = 0, erasec = 0, appler = 0, applec = 0, delay = 0;
  while (1) {
    currentButtons = BUTTONS;  

    switch (state) {
      case START:

        if (!initialized)
        {
          length = 1; 
          snake[0].r = GAME_HEIGHT / (2 * SCALE);
          snake[0].c = ORIGINAL_LENGTH - 1;
          snake[0].dr = 0;
          snake[0].dc = 1;
          for (int i = 0; i < ORIGINAL_LENGTH - 1; i++)
            length = grow(snake, length);
          erase = 0;
          appler = GAME_HEIGHT / (2 * SCALE);
          applec = GAME_WIDTH / (2 * SCALE);
          delay = ORIGINAL_DELAY;
          initialized = 1;
        }
        waitForVBlank();
        drawFullScreenImageDMA(start);
        if (KEY_DOWN(BUTTON_START, currentButtons)) 
        {
          state = PLAY;
          waitForVBlank();
          fillScreenDMA(BLACK);
          drawRectDMA(snake[length - 1].r * SCALE, snake[length - 1].c * SCALE, length * SCALE, SCALE, WHITE);
          drawRectDMA(appler * SCALE, applec * SCALE, SCALE, SCALE, RED);
          drawImageDMA(GAME_HEIGHT, 60, WIDTH - 60, HEIGHT - GAME_HEIGHT, middle);
          updateScore(length);
        }
        break;
      case PLAY:

        if (snake[0].r < 0 || snake[0].r * SCALE >= GAME_HEIGHT || snake[0].c < 0 || snake[0].c * SCALE >= GAME_WIDTH || collision(snake + 1, length - 1, snake[0].r, snake[0].c))
        {
          state = LOSE;
          break;
        }
        int updated = 0;  
        for (int i = 0; i <= delay; i++)
        {
            if (KEY_DOWN(BUTTON_SELECT, currentButtons))
            {
                initialized = 0;
                state = START;
                break;
            }
            if (!updated)
            {
                if (snake[0].dc == 0) 
                {
                  if (KEY_DOWN(BUTTON_LEFT, currentButtons))
                  {
                    snake[0].dr = 0;
                    snake[0].dc = -1;
                    updated = 1;
                  }
                  else if (KEY_DOWN(BUTTON_RIGHT, currentButtons))
                  {
                    snake[0].dr = 0;
                    snake[0].dc = 1;
                    updated = 1;
                  }
                }
                else if (snake[0].dr == 0)
                {
                  if (KEY_DOWN(BUTTON_UP, currentButtons))
                  {
                    snake[0].dr = -1;
                    snake[0].dc = 0;
                    updated = 1;
                  }
                  else if (KEY_DOWN(BUTTON_DOWN, currentButtons))
                  {
                    snake[0].dr = 1;
                    snake[0].dc = 0;
                    updated = 1;
                  }
                }
            }
            waitForVBlank();
        }
        if (state != PLAY) break;
        if (erase)
          drawRectDMA(eraser * SCALE, erasec * SCALE, SCALE, SCALE, BLACK);
        erase = 1;
        drawRectDMA(snake[0].r * SCALE, snake[0].c * SCALE, SCALE, SCALE, WHITE);
        updateScore(length);
        eraser = snake[length - 1].r;
        erasec = snake[length - 1].c;
        move(snake, length);
        if (snake[0].r == appler && snake[0].c == applec)
        {
          length = grow(snake, length);
          if (delay > 0) delay--;
          placeApple(snake, length, &appler, &applec);
          drawRectDMA(appler * SCALE, applec * SCALE, SCALE, SCALE, RED);
          erase = 0;
        }
        updateDirections(snake, length);
        break;
      case WIN:

        break;
      case LOSE:
        waitForVBlank();
        drawFullScreenImageDMA(lose);
        if (KEY_DOWN(BUTTON_SELECT, currentButtons))
        {
            initialized = 0;
            state = START;
            break;
        }
        break;
    }

  }

  return 0;
}
