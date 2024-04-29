#include <cmath>
#include <vector>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "spaceObject.h"

#define FPS 60
#define WIDTH 1000
#define HEIGHT 1000
#define SHIP_SPIN 0.15
#define SHIP_ACCL 0.08
#define BULLET_SPEED 1.5

#define SHIP 1
#define ASTEROID 2
#define BULLET 3

SDL_Window *window;
SDL_Renderer *renderer;

std::vector<asteroid *> asteroids;
std::vector<spaceObject *> bullets;
spaceObject *ship;

point_t shipFrame[3];
int numSpawnAsteroids = 2;

bool running = false;
int centerX, centerY;

void placeAsteroids(){
	int i;
	for(i = 0; i < numSpawnAsteroids; i++){
		asteroids.push_back(new asteroid(ship->x + (-1 * (rand() % 2 + 1)) * ((rand() % 12) + 30), ship->y + (-1 * (rand() % 2 + 1)) * (17), (-1 * (rand() % 2 + 1)) * 0.1, (-1 * (rand() % 2 + 1)) * 0.05, 0.05, 16));
	}
	numSpawnAsteroids++;
}

void endGame(){
	delete ship;
	std::vector<asteroid *>::iterator ai = asteroids.begin();
	for(; ai != asteroids.end(); ai++){
		delete *ai;
	}	
	std::vector<spaceObject *>::iterator bi = bullets.begin();
	for(; bi != bullets.end(); bi++){
		delete *bi;
	}	
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    running = false;
}

void drawWireFrame(int x, int y, point_t frame[], int frameSize, float angle, int color){
	point_t rotatedFrame[frameSize];
	
	int i;
	for(i = 0; i < frameSize; i++){
		rotatedFrame[i].x = (frame[i].x * cos(angle) - frame[i].y * sin(angle));
		rotatedFrame[i].y = frame[i].x * sin(angle) + frame[i].y * cos(angle);
	}
	for(i = 0; i < frameSize - 1; i++){
		SDL_RenderDrawLine(renderer, x + rotatedFrame[i].x, y + rotatedFrame[i].y, x + rotatedFrame[i + 1].x, y + rotatedFrame[i + 1].y);
	}

	SDL_RenderDrawLine(renderer, x + rotatedFrame[0].x, y + rotatedFrame[0].y, x + rotatedFrame[frameSize - 1].x, y + rotatedFrame[frameSize - 1].y);
}

void drawShip(){
	drawWireFrame(ship->x, ship->y, shipFrame, 3, ship->angle, SHIP);
}

void drawAsteroids(){
	if(asteroids.empty()) return;
	std::vector<asteroid *>::iterator ai;
	for(ai = asteroids.begin(); ai != asteroids.end(); ai++){
		drawWireFrame((*ai)->x, (*ai)->y, (*ai)->frame, 8, (*ai)->angle, ASTEROID);
	}
}

void drawBullets(){
	if(bullets.empty()) return;
	std::vector<spaceObject *>::iterator si;
	for(si = bullets.begin(); si != bullets.end(); si++){
        SDL_RenderDrawPoint(renderer, (*si)->x, (*si)->y);
    }
}

void updateGame(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode){
                    case SDL_SCANCODE_Q:
                        running = false;
                        break;

                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        ship->dx += SHIP_ACCL * sin(ship->angle);
                        ship->dy -= SHIP_ACCL * cos(ship->angle);
                        break;

                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT: 
		                ship->angle -= SHIP_SPIN;
                        break;

                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
		                ship->angle += SHIP_SPIN;
                        break;

                    case SDL_SCANCODE_SPACE:
		                bullets.push_back(new spaceObject(ship->x, ship->y, BULLET_SPEED * sin(ship->angle), -1 * BULLET_SPEED * cos(ship->angle), 0));
                        break;

                    default:
                        break;
                }
        }
    }
	
	ship->x += ship->dx;
	ship->y += ship->dy;

	if(ship->x > WIDTH) ship->x -= WIDTH;
	if(ship->x < 0) ship->x += WIDTH;
	if(ship->y > HEIGHT) ship->y -= HEIGHT;
	if(ship->y < 0) ship->y += HEIGHT;
	
	std::vector<spaceObject *>::iterator si;
	for(si = bullets.begin(); si != bullets.end(); si++){
		(*si)->x += (*si)->dx;
		(*si)->y += (*si)->dy;
		if((*si)->x > WIDTH || (*si)->x < 0 || (*si)->y > HEIGHT || (*si)->y < 0){
			//remove bullet from vector
			delete (*si);
			bullets.erase(si);
			si--;
		}
	}
	
	if(!asteroids.empty()){
		std::vector<asteroid *>::iterator ai;
		std::vector<asteroid *> tempAst;
		for(ai = asteroids.begin(); ai != asteroids.end(); ai++){
			(*ai)->x += (*ai)->dx;
			(*ai)->y += (*ai)->dy;
			if((*ai)->x > WIDTH) (*ai)->x -= WIDTH;
			if((*ai)->x < 0) (*ai)->x += WIDTH;
			if((*ai)->y > HEIGHT) (*ai)->y -= HEIGHT;
			if((*ai)->y < 0) (*ai)->y += HEIGHT;
			
			//collided with player
			if((*ai)->size * (*ai)->size >= ((ship->x - (*ai)->x) * (ship->x - (*ai)->x)) + ((ship->y - (*ai)->y) * (ship->y - (*ai)->y))){
                running = false;
			}

			for(si = bullets.begin(); si != bullets.end(); si++){
				if((*ai)->size * (*ai)->size >= (((*si)->x - (*ai)->x) * ((*si)->x - (*ai)->x)) + (((*si)->y - (*ai)->y) * ((*si)->y - (*ai)->y))){
					//bullet hit asteroid
					delete (*si);
					bullets.erase(si);
					si--;
					if((*ai)->size > 4){
						tempAst.push_back(new asteroid((*ai)->x, (*ai)->y, (*ai)->dx + static_cast<double> (rand() / static_cast<double> (RAND_MAX)) - 0.5, (*ai)->dy + static_cast<double> (rand() / static_cast<double> (RAND_MAX)) - 0.5, -0.05, (*ai)->size / 2));
						tempAst.push_back(new asteroid((*ai)->x, (*ai)->y, (*ai)->dx + static_cast<double> (rand() / static_cast<double> (RAND_MAX)) - 0.5, (*ai)->dy + static_cast<double> (rand() / static_cast<double> (RAND_MAX)) - 0.5, 0.1, (*ai)->size / 2));
					}
					delete (*ai);
					asteroids.erase(ai);
					ai--;
					break;
				}
			}
		}

		for(ai = tempAst.begin(); ai != tempAst.end(); ai++){
			asteroids.push_back(*ai);
		}
	}else{
		placeAsteroids();		
	}
}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawShip();
	drawAsteroids();
	drawBullets();
    SDL_RenderPresent(renderer);
}

void gameLoop(){
	uint64_t frameStart;  
    uint64_t frameTime;
    uint64_t frameDelay = 1000 / FPS;

	while(running){
        frameStart = SDL_GetTicks64();
        
        updateGame();
		render();

        frameTime = SDL_GetTicks64() - frameStart;

        if(frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}	
}

int startGame(){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
       return -1;
    }

    window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ship = new spaceObject(centerX, centerY, 0, 0, 0);
	placeAsteroids();
	shipFrame[0].x = 0;
	shipFrame[0].y = -5;
	shipFrame[1].x = 2;
	shipFrame[1].y = 2;
	shipFrame[2].x = -2;
	shipFrame[2].y = 2;
    running = true;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    return 0;
}

int main(int argc, char * argv[]){
	if(startGame() != 0){
       printf("Error initializing SDL: %s", SDL_GetError()); 
       return -1;
    }
	
	gameLoop();

	endGame();
	return 0;
}
