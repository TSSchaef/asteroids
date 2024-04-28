#ifndef SPACE_OBJECT_H
#define SPACE_OBJECT_H


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
	point_t frame[8];
	asteroid(float x, float y, float dx, float dy, float angle, int size) : spaceObject(x, y, dx, dy, angle){
		this->size = size;

		int i;
		for(i = 0; i < 8; i++){
			frame[i].x = size * cos(i * (3.14159 / 4)) + (rand() % 3 - 1);
			frame[i].y = size * sin(i * (3.14159 / 4)) + (rand() % 3 - 1);
		}
	}
	~asteroid(){}
};
#endif
