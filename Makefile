#Makefile

asteroids: asteroids.o 
	g++ asteroids.o -o asteroids -lSDL2 -lSDL2_ttf -lSDL2_image

asteroids.o: src/asteroids.cpp src/spaceObject.h
	g++ -Wall -Werror -g src/asteroids.cpp -c

.PHONY: clean

clean:
	rm -f *.o core* asteroids valgrind*
