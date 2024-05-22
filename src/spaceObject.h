#ifndef SPACE_OBJECT_H
#define SPACE_OBJECT_H

//#define ASTEROID_RESOLUTION 16

extern SDL_Renderer *renderer;

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define ASTEROID1 "src/images/asteroid-32-1.png"
#define ASTEROID2 "src/images/asteroid-32-2.png"

typedef struct point{
	int x, y;
} point_t;

class spaceObject{
public:
	float x, y, dx, dy, angle;
	spaceObject(float x, float y, float dx, float dy, float angle){
		this->x = x;
		this->y = y;
		this->dx = dx;
		this->dy = dy;
		this->angle = angle;
	}
	~spaceObject(){}
};

class asteroid : public spaceObject{
public:
	int size;
    SDL_Texture *texture;
	//point_t frame[ASTEROID_RESOLUTION];
	asteroid(float x, float y, float dx, float dy, float angle, int size) : spaceObject(x, y, dx, dy, angle){
        
        int rnd = (rand() % 2) + 1;
        switch(rnd){
            case 1:
                texture = IMG_LoadTexture(renderer, ASTEROID1);
                break;
            case 2:
                texture = IMG_LoadTexture(renderer, ASTEROID2);
                break;
            default:
                texture = IMG_LoadTexture(renderer, ASTEROID1);
        }

		this->size = size;

	/*	int i;
		for(i = 0; i < ASTEROID_RESOLUTION; i++){
			frame[i].x = size * cos(i * (3.14159 / (ASTEROID_RESOLUTION / 2))) + (rand() % 3 - 1);
			frame[i].y = size * sin(i * (3.14159 / (ASTEROID_RESOLUTION / 2))) + (rand() % 3 - 1);
		}*/
	}
	~asteroid(){
        SDL_DestroyTexture(texture);
    }
};
#endif
