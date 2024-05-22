#include <cstring>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "spaceObject.h"

#define FPS 60
#define WIDTH 1000
#define HEIGHT 1000
#define SHIP_SPIN 8.0 / FPS 
#define SHIP_ACCL 3.0 / FPS 
#define BULLET_SPEED 240.0 / FPS 
#define BULLET_DELAY 250

#define SHIP_SIZE 64
#define MIN_ASTEROID_SIZE 32
#define MAX_ASTEROID_SIZE 128 

#define SHIP "src/images/ship.png"
#define SHIP_BOOSTING "src/images/ship-boosting.png"

#define BACKGROUND_FLICKER 1000 
#define BCKGRND1 "src/images/background1.png"
#define BCKGRND2 "src/images/background2.png"

SDL_Window *window;
SDL_Renderer *renderer;

std::vector<asteroid *> asteroids;
std::vector<spaceObject *> bullets;
spaceObject *ship;

//point_t shipFrame[3];
int numSpawnAsteroids = 2;
int score = 0;

bool keysPressed[SDL_NUM_SCANCODES];

bool running = false;
bool isBoosting = false;

SDL_Color White = {255, 255, 255, 255};

SDL_Texture *shipTex;
SDL_Texture *shipBoostingTex;

SDL_Texture *bckgrnd1;
SDL_Texture *bckgrnd2;

SDL_Rect bckgrndRect = {0, 0, WIDTH, HEIGHT};

TTF_Font* Hyperspace; 
TTF_Font* HyperspaceScore; 
TTF_Font* HyperspaceTitle; 

void placeAsteroids(){
	int i;
	for(i = 0; i < numSpawnAsteroids; i++){
        int x, y;
        while(true){
            x = rand() % WIDTH;
            y = rand() % HEIGHT;

			if(MAX_ASTEROID_SIZE * MAX_ASTEROID_SIZE < ((ship->x - x) * (ship->x - x)) + ((ship->y - y) * (ship->y - y))) break;
        }
		asteroids.push_back(new asteroid(x, y, static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, 0.05, MAX_ASTEROID_SIZE));
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
    asteroids.clear();
    bullets.clear();
    running = false;
}

/*void drawWireFrame(int x, int y, point_t frame[], int frameSize, float angle){
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
}*/

void drawShip(){
	//drawWireFrame(ship->x, ship->y, shipFrame, 3, ship->angle);
    SDL_Rect shipRect = {(int)ship->x - (SHIP_SIZE / 2), (int)ship->y - (SHIP_SIZE / 2), SHIP_SIZE, SHIP_SIZE};
    float angle = ship->angle * 180 / 3.1415926;
    if(isBoosting){
        SDL_RenderCopyEx(renderer, shipBoostingTex, NULL, &shipRect, angle, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyEx(renderer, shipTex, NULL, &shipRect, angle, NULL, SDL_FLIP_NONE);
    }
}

void drawAsteroids(){
	if(asteroids.empty()) return;
	std::vector<asteroid *>::iterator ai;
	for(ai = asteroids.begin(); ai != asteroids.end(); ai++){
        int size = (*ai)->size;
        SDL_Rect rect = {(int)(*ai)->x - (size / 2), (int)(*ai)->y - (size / 2), size, size};
        float angle = (*ai)->angle * 180 / 3.1415926;
        SDL_RenderCopyEx(renderer, (*ai)->texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
       // drawWireFrame((*ai)->x, (*ai)->y, (*ai)->frame, ASTEROID_RESOLUTION, (*ai)->angle);
	}
}

void drawBullets(){
	if(bullets.empty()) return;
	std::vector<spaceObject *>::iterator si;
	for(si = bullets.begin(); si != bullets.end(); si++){
        SDL_RenderDrawPoint(renderer, (*si)->x, (*si)->y);
    }
}

void showScore(){
    std::string msg = "Score: " + std::to_string(score); 
    SDL_Surface *surfaceMessage =
        TTF_RenderText_Solid(HyperspaceScore, msg.c_str(), White);
    SDL_Texture *Message = 
        SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    int w, h;
    SDL_QueryTexture(Message, NULL, NULL, &w, &h);
    SDL_Rect rect = {0, 0, w, h};

    SDL_RenderCopy(renderer, Message, NULL, &rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

int userInput(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                return 0;
                running = false;
                break;

            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                        running = false;
                }
                keysPressed[event.key.keysym.scancode] = true;
            break;

            case SDL_KEYUP:
                keysPressed[event.key.keysym.scancode] = false;
            break;
        }
    }
    return 1;
}

void updateGame(){
    static uint64_t lastBulletFired;
	
    if(keysPressed[SDL_SCANCODE_W] || keysPressed[SDL_SCANCODE_UP]){
        ship->dx += SHIP_ACCL * sin(ship->angle);
        ship->dy -= SHIP_ACCL * cos(ship->angle);
        isBoosting = true;
    } else {
        isBoosting = false;
    }

    if(keysPressed[SDL_SCANCODE_A] || keysPressed[SDL_SCANCODE_LEFT]){
        ship->angle -= SHIP_SPIN;
    }

    if(keysPressed[SDL_SCANCODE_D] || keysPressed[SDL_SCANCODE_RIGHT]){
        ship->angle += SHIP_SPIN;
    }

    if(keysPressed[SDL_SCANCODE_SPACE] && SDL_GetTicks64() > lastBulletFired + BULLET_DELAY){
        bullets.push_back(new spaceObject(ship->x, ship->y, ship->dx + BULLET_SPEED * sin(ship->angle), ship->dy + -1 * BULLET_SPEED * cos(ship->angle), 0));
        lastBulletFired = SDL_GetTicks64();
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

            (*ai)->angle += ((*ai)->angle < 0) ? -0.01 : 0.01;
			
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

                    score += (*ai)->size * 6.25;
					if((*ai)->size > MIN_ASTEROID_SIZE){
						tempAst.push_back(new asteroid((*ai)->x, (*ai)->y, (*ai)->dx + static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, (*ai)->dy + static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, -0.05, (*ai)->size / 2));
						tempAst.push_back(new asteroid((*ai)->x, (*ai)->y, (*ai)->dx + static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, (*ai)->dy + static_cast<double> (rand() / static_cast<double> (0.5 * RAND_MAX)) - 1, 0.1, (*ai)->size / 2));
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
        score += 250 * numSpawnAsteroids;
		placeAsteroids();		
	}
}

void drawBackground(){
    static uint64_t lastSwap = 0;
    static bool swap = true;

    if(SDL_GetTicks64() > lastSwap + BACKGROUND_FLICKER){
        lastSwap = SDL_GetTicks64();
        swap = !swap;
    }

    if(swap){
        SDL_RenderCopy(renderer, bckgrnd1, NULL, &bckgrndRect);
    } else {
        SDL_RenderCopy(renderer, bckgrnd2, NULL, &bckgrndRect);
    }
}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    drawBackground();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawShip();
    showScore();
    //SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
	drawAsteroids();
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	drawBullets();
    SDL_RenderPresent(renderer);
}

void gameLoop(){
	uint64_t frameStart;  
    uint64_t frameTime;
    uint64_t frameDelay = 1000 / FPS;

	while(running){
        frameStart = SDL_GetTicks64();
       
        userInput();
        updateGame();
		render();

        frameTime = SDL_GetTicks64() - frameStart;

        if(frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}	
}

void startGame(){
	ship = new spaceObject(WIDTH / 2, HEIGHT / 2, 0, 0, 0);
	placeAsteroids();
	/*shipFrame[0].x = 0;
	shipFrame[0].y = -10;
	shipFrame[1].x = 4;
	shipFrame[1].y = 4;
	shipFrame[2].x = -4;
	shipFrame[2].y = 4;*/
    score = 0;
    numSpawnAsteroids = 2;
    running = true;
}

int startScreen(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
       return -1;
    }

    window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bckgrnd1 = IMG_LoadTexture(renderer, BCKGRND1);
    bckgrnd2 = IMG_LoadTexture(renderer, BCKGRND2);

    shipTex = IMG_LoadTexture(renderer, SHIP);
    shipBoostingTex = IMG_LoadTexture(renderer, SHIP_BOOSTING);

    TTF_Init();
    Hyperspace = TTF_OpenFont("src/Hyperspace.ttf", 24);
    HyperspaceScore = TTF_OpenFont("src/Hyperspace.ttf", 16);
    HyperspaceTitle = TTF_OpenFont("src/Hyperspace.ttf", 128);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    return 0;
}

void endScreen(){
    TTF_CloseFont(Hyperspace);
    TTF_CloseFont(HyperspaceTitle);
    TTF_Quit();

    SDL_DestroyTexture(shipTex);
    SDL_DestroyTexture(shipBoostingTex);
    SDL_DestroyTexture(bckgrnd1);
    SDL_DestroyTexture(bckgrnd2);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool titleScreen(){
	uint64_t frameStart;  
    uint64_t frameTime;
    uint64_t frameDelay = 1000 / FPS;

    int index = 0;
    bool justMoved = false;

    int menuHeight = 4; //Play - Options - Help - Quit

    SDL_Surface *title;
    SDL_Surface *play;
    SDL_Surface *options;
    SDL_Surface *help;
    SDL_Surface *quit;
    
    SDL_Texture *titleTxt;
    SDL_Texture *playTxt;
    SDL_Texture *optionsTxt;
    SDL_Texture *helpTxt;
    SDL_Texture *quitTxt;
    
    SDL_Rect playRect;
    SDL_Rect titleRect;
    SDL_Rect optionsRect;
    SDL_Rect helpRect;
    SDL_Rect quitRect;

    int w, h;
	
    while(true){
        frameStart = SDL_GetTicks64();
       
        if(!userInput()){
            return false;
        }

        if((keysPressed[SDL_SCANCODE_S] || keysPressed[SDL_SCANCODE_DOWN]) && !(keysPressed[SDL_SCANCODE_W] || keysPressed[SDL_SCANCODE_UP])){
           if(!justMoved && index < menuHeight) index++;
           justMoved = true;
        } else if(!(keysPressed[SDL_SCANCODE_S] || keysPressed[SDL_SCANCODE_DOWN]) && (keysPressed[SDL_SCANCODE_W] || keysPressed[SDL_SCANCODE_UP])){
           if(!justMoved && index > 0) index--;
            justMoved = true;
        } else {
            justMoved = false;
        }

        if(keysPressed[SDL_SCANCODE_SPACE] || keysPressed[SDL_SCANCODE_RETURN]){
            switch(index){
                case 0:
                    return true;
                case 1:
        //            options();
                    break;
                case 2:
         //           help();
                    break;
                case 3:
                    return false;
            }
        }

        title = TTF_RenderText_Solid(HyperspaceTitle, "Asteroids", White);
        titleTxt = SDL_CreateTextureFromSurface(renderer, title);
        SDL_QueryTexture(titleTxt, NULL, NULL, &w, &h);
        titleRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) - h - 125, w, h};

        if(index == 0){
            play = TTF_RenderText_Solid(Hyperspace, "XX Play XX", White);
            playTxt = SDL_CreateTextureFromSurface(renderer, play);
            SDL_QueryTexture(playTxt, NULL, NULL, &w, &h);
            playRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2), w, h};
            
            options = TTF_RenderText_Solid(Hyperspace, "Options", White);
            optionsTxt = SDL_CreateTextureFromSurface(renderer, options);
            SDL_QueryTexture(optionsTxt, NULL, NULL, &w, &h);
            optionsRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + h + 5, w, h};
            
            help = TTF_RenderText_Solid(Hyperspace, "Help", White);
            helpTxt = SDL_CreateTextureFromSurface(renderer, help);
            SDL_QueryTexture(helpTxt, NULL, NULL, &w, &h);
            helpRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 2*(h + 5), w, h};
            
            quit = TTF_RenderText_Solid(Hyperspace, "Quit", White);
            quitTxt = SDL_CreateTextureFromSurface(renderer, quit);
            SDL_QueryTexture(quitTxt, NULL, NULL, &w, &h);
            quitRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 3*(h + 5), w, h};
        } else if (index == 1){
            play = TTF_RenderText_Solid(Hyperspace, "Play", White);
            playTxt = SDL_CreateTextureFromSurface(renderer, play);
            SDL_QueryTexture(playTxt, NULL, NULL, &w, &h);
            playRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2), w, h};
            
            options = TTF_RenderText_Solid(Hyperspace, "XX Options XX", White);
            optionsTxt = SDL_CreateTextureFromSurface(renderer, options);
            SDL_QueryTexture(optionsTxt, NULL, NULL, &w, &h);
            optionsRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + h + 5, w, h};
            
            help = TTF_RenderText_Solid(Hyperspace, "Help", White);
            helpTxt = SDL_CreateTextureFromSurface(renderer, help);
            SDL_QueryTexture(helpTxt, NULL, NULL, &w, &h);
            helpRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 2*(h + 5), w, h};
            
            quit = TTF_RenderText_Solid(Hyperspace, "Quit", White);
            quitTxt = SDL_CreateTextureFromSurface(renderer, quit);
            SDL_QueryTexture(quitTxt, NULL, NULL, &w, &h);
            quitRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 3*(h + 5), w, h};
        } else if (index == 2){
            play = TTF_RenderText_Solid(Hyperspace, "Play", White);
            playTxt = SDL_CreateTextureFromSurface(renderer, play);
            SDL_QueryTexture(playTxt, NULL, NULL, &w, &h);
            playRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2), w, h};
            
            options = TTF_RenderText_Solid(Hyperspace, "Options", White);
            optionsTxt = SDL_CreateTextureFromSurface(renderer, options);
            SDL_QueryTexture(optionsTxt, NULL, NULL, &w, &h);
            optionsRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + h + 5, w, h};
            
            help = TTF_RenderText_Solid(Hyperspace, "XX Help XX", White);
            helpTxt = SDL_CreateTextureFromSurface(renderer, help);
            SDL_QueryTexture(helpTxt, NULL, NULL, &w, &h);
            helpRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 2*(h + 5), w, h};
            
            quit = TTF_RenderText_Solid(Hyperspace, "Quit", White);
            quitTxt = SDL_CreateTextureFromSurface(renderer, quit);
            SDL_QueryTexture(quitTxt, NULL, NULL, &w, &h);
            quitRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 3*(h + 5), w, h};
        } else {
            play = TTF_RenderText_Solid(Hyperspace, "Play", White);
            playTxt = SDL_CreateTextureFromSurface(renderer, play);
            SDL_QueryTexture(playTxt, NULL, NULL, &w, &h);
            playRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2), w, h};
            
            options = TTF_RenderText_Solid(Hyperspace, "Options", White);
            optionsTxt = SDL_CreateTextureFromSurface(renderer, options);
            SDL_QueryTexture(optionsTxt, NULL, NULL, &w, &h);
            optionsRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + h + 5, w, h};
            
            help = TTF_RenderText_Solid(Hyperspace, "Help", White);
            helpTxt = SDL_CreateTextureFromSurface(renderer, help);
            SDL_QueryTexture(helpTxt, NULL, NULL, &w, &h);
            helpRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 2*(h + 5), w, h};
            
            quit = TTF_RenderText_Solid(Hyperspace, "XX Quit XX", White);
            quitTxt = SDL_CreateTextureFromSurface(renderer, quit);
            SDL_QueryTexture(quitTxt, NULL, NULL, &w, &h);
            quitRect = {(WIDTH / 2) - (w / 2), (HEIGHT / 2) + 3*(h + 5), w, h};
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        drawBackground();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, titleTxt, NULL, &titleRect);
        SDL_RenderCopy(renderer, playTxt, NULL, &playRect);
        SDL_RenderCopy(renderer, optionsTxt, NULL, &optionsRect);
        SDL_RenderCopy(renderer, helpTxt, NULL, &helpRect);
        SDL_RenderCopy(renderer, quitTxt, NULL, &quitRect);

        SDL_RenderPresent(renderer);

        SDL_FreeSurface(title);
        SDL_DestroyTexture(titleTxt);
        SDL_FreeSurface(play);
        SDL_DestroyTexture(playTxt);
        SDL_FreeSurface(options);
        SDL_DestroyTexture(optionsTxt);
        SDL_FreeSurface(help);
        SDL_DestroyTexture(helpTxt);
        SDL_FreeSurface(quit);
        SDL_DestroyTexture(quitTxt);

        frameTime = SDL_GetTicks64() - frameStart;

        if(frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}	
}

int main(int argc, char * argv[]){
	if(startScreen() != 0){
       printf("Error initializing SDL: %s", SDL_GetError()); 
       return -1;
    }

    while(titleScreen()){
        startGame();
        gameLoop();
        endGame();
    }

    endScreen();

	return 0;
}
