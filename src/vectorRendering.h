#ifndef VECTOR_RENDERING_H
#define VECTOR_RENDERING_H

#include <SDL2/SDL.h>

#define ASTEROID 1
#define BULLET 2
#define EXPLOSION 3
#define SHIP 4
#define DEFAULT 5

int startScreen();
void endScreen();
void draw(int x, int y, int color);

void drawLine(int x1, int y1, int x2, int y2, int color);


#endif
