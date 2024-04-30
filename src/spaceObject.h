#ifndef SPACE_OBJECT_H
#define SPACE_OBJECT_H

#define ASTEROID_RESOLUTION 16

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
	point_t frame[ASTEROID_RESOLUTION];
	asteroid(float x, float y, float dx, float dy, float angle, int size) : spaceObject(x, y, dx, dy, angle){
		this->size = size;

		int i;
		for(i = 0; i < ASTEROID_RESOLUTION; i++){
			frame[i].x = size * cos(i * (3.14159 / (ASTEROID_RESOLUTION / 2))) + (rand() % 3 - 1);
			frame[i].y = size * sin(i * (3.14159 / (ASTEROID_RESOLUTION / 2))) + (rand() % 3 - 1);
		}
	}
	~asteroid(){}
};
#endif
