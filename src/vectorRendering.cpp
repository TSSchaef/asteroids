#include "vectorRendering.h"

int startScreen(){
	initscr();
	if(COLS < 130 || LINES < 40){
		endwin();
	       	return 0;
	}
	raw();
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(SHIP, COLOR_WHITE, COLOR_WHITE);
	init_pair(ASTEROID, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(BULLET, COLOR_RED, COLOR_RED);
	init_pair(EXPLOSION, COLOR_RED, COLOR_YELLOW);
	init_pair(DEFAULT, COLOR_WHITE, COLOR_BLACK);
	return 1;
}

void endScreen(){
	endwin();
}

void draw(int x, int y, int color){
	attron(COLOR_PAIR(SHIP));
	mvaddch(y, x, 'X');
}

void drawLineSegment(int x, int y){
	if(x > COLS) x -= COLS;
	if(x < 0) x += COLS;
	if(y > LINES) y -= LINES;
	if(y < 0) y += LINES;
	mvaddch(y, x, 'X');
}


void drawLowLine(int x1, int y1, int x2, int y2){
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	if( dy < 0 ){
		yi = -1;
		dy = -1 * dy;
	}

	int D = (2 * dy) - dx;
	int y = y1;
	int x;
	for(x = x1; x <= x2; x++){
		drawLineSegment(x, y);
		if(D > 0){
			y += yi;
			D += 2 * (dy - dx);
		}else{
			D += 2*dy;
		}		
	}
}

void drawHighLine(int x1, int y1, int x2, int y2){
	int dx = x2 - x1;
	int dy = y2 - y1;
	int xi = 1;
	if( dx < 0 ){
		xi = -1;
		dx = -1 * dx;
	}

	int D = (2 * dx) - dy;
	int x = x1;
	int y;
	for(y = y1; y <= y2; y++){
		drawLineSegment(x, y);
		if(D > 0){
			x += xi;
			D += 2 * (dx - dy);
		}else{
			D += 2*dx;
		}		
	}
}

//using bressenham's line drawing alg
void drawLine(int x1, int y1, int x2, int y2, int color){
	attron(COLOR_PAIR(color));
	int yDif = (y1 - y2 < 0)? -1 *(y1 - y2): (y1 - y2);	
	int xDif = (x1 - x2 < 0)? -1 *(x1 - x2): (x1 - x2);	

	if(yDif < xDif){
		if(x1 > x2){
			drawLowLine(x2, y2, x1, y1);
		}else{
			drawLowLine(x1, y1, x2, y2);
		}	
	}else{
		if(y1 > y2){
			drawHighLine(x2, y2, x1, y1);
		}else{
			drawHighLine(x1, y1, x2, y2);
		}	
	}
}
