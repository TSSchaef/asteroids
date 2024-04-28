#include <vector>
#include <cmath>
#include <chrono>
#include <cstring>

#include "spaceObject.h"
#include "vectorRendering.h"

#define X_FACTOR 1.6
#define SHIP_SPIN 0.15
#define SHIP_ACCL 0.08
#define BULLET_SPEED 1.5

std::vector<asteroid *> asteroids;
std::vector<spaceObject *> bullets;
spaceObject *ship;

point_t shipFrame[3];
int numSpawnAsteroids = 2;

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
}

void drawWireFrame(int x, int y, point_t frame[], int frameSize, float angle, int color){
	point_t rotatedFrame[frameSize];
	
	int i;
	for(i = 0; i < frameSize; i++){
		rotatedFrame[i].x = X_FACTOR * (frame[i].x * cos(angle) - frame[i].y * sin(angle));
		rotatedFrame[i].y = frame[i].x * sin(angle) + frame[i].y * cos(angle);
	}
	for(i = 0; i < frameSize - 1; i++){
		drawLine(x + rotatedFrame[i].x, y + rotatedFrame[i].y, x + rotatedFrame[i + 1].x, y + rotatedFrame[i + 1].y, color);
	}

	drawLine( x + rotatedFrame[0].x, y + rotatedFrame[0].y, x + rotatedFrame[frameSize - 1].x, y + rotatedFrame[frameSize - 1].y, color);
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
		draw((*si)->x, (*si)->y, BULLET);
	}
}

int updateGame(uint64_t delta){
	char ch[10];
	getnstr(ch, 10);
	if(strchr(ch, 'Q') != NULL){
		return 1;
	}
	
	if(strchr(ch, 'a') != NULL){
		ship->angle -= SHIP_SPIN * delta * delta;
	}	
	
	if(strchr(ch, 'd') != NULL){
		ship->angle += SHIP_SPIN * delta * delta;
	}	

	if(strchr(ch, 'w') != NULL){
		ship->dx += SHIP_ACCL * sin(ship->angle) * delta / 10;
		ship->dy -= SHIP_ACCL * cos(ship->angle) * delta / 10;
	}	

	ship->x += ship->dx * delta;
	ship->y += ship->dy * delta;

	if(ship->x > COLS) ship->x -= COLS;
	if(ship->x < 0) ship->x += COLS;
	if(ship->y > LINES) ship->y -= LINES;
	if(ship->y < 0) ship->y += LINES;
	
	if(strchr(ch, ' ') != NULL){
		bullets.push_back(new spaceObject(ship->x, ship->y, BULLET_SPEED * sin(ship->angle), -1 * BULLET_SPEED * cos(ship->angle), 0));
	}

	std::vector<spaceObject *>::iterator si;
	for(si = bullets.begin(); si != bullets.end(); si++){
		(*si)->x += (*si)->dx * delta / 10;
		(*si)->y += (*si)->dy * delta / 10;
		if((*si)->x > COLS || (*si)->x < 0 || (*si)->y > LINES || (*si)->y < 0){
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
			(*ai)->x += (*ai)->dx * delta / 10;
			(*ai)->y += (*ai)->dy * delta / 10;
			if((*ai)->x > COLS) (*ai)->x -= COLS;
			if((*ai)->x < 0) (*ai)->x += COLS;
			if((*ai)->y > LINES) (*ai)->y -= LINES;
			if((*ai)->y < 0) (*ai)->y += LINES;
			
			//collided with player
			if((*ai)->size * (*ai)->size >= (X_FACTOR * X_FACTOR * (ship->x - (*ai)->x) * (ship->x - (*ai)->x)) + ((ship->y - (*ai)->y) * (ship->y - (*ai)->y))){
				return 2;
			}

			for(si = bullets.begin(); si != bullets.end(); si++){
				if((*ai)->size * (*ai)->size >= (X_FACTOR * X_FACTOR * ((*si)->x - (*ai)->x) * ((*si)->x - (*ai)->x)) + (((*si)->y - (*ai)->y) * ((*si)->y - (*ai)->y))){
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
	return 0;
}

void render(){
	erase();
	drawShip();
	drawAsteroids();
	drawBullets();
	refresh();
}

uint64_t getCurrTime(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void gameLoop(){
	uint64_t previous = getCurrTime();
	
	while(true){
		uint64_t current = getCurrTime();
		uint64_t elapsed = current - previous;
		previous = current;

		if(updateGame(elapsed)) break;

		render();
	}	
}

void startGame(){
	//COLS and LINES are set by ncurses on init
	//they are also altered on screen resizing
	srand(getCurrTime());
	centerX = COLS / 2;
	centerY = LINES / 2;
	ship = new spaceObject(centerX, centerY, 0, 0, 0);
	placeAsteroids();
	shipFrame[0].x = 0;
	shipFrame[0].y = -5;
	shipFrame[1].x = 2;
	shipFrame[1].y = 2;
	shipFrame[2].x = -2;
	shipFrame[2].y = 2;
}

int main(int argc, char * argv[]){
	if(!startScreen()){
		printf("Screen resolution is too low, see README\n");       
		return 0;
	}
	startGame();
	
	gameLoop();

	endGame();
	endScreen();
	return 0;
}
