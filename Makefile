#Makefile

asteroids: asteroids.o vectorRendering.o
	g++ asteroids.o vectorRendering.o -o asteroids -lncurses

asteroids.o: src/asteroids.cpp src/spaceObject.h src/vectorRendering.h
	g++ -Wall -Werror -g src/asteroids.cpp -c

vectorRendering.o: src/vectorRendering.h src/vectorRendering.cpp
	g++ -Wall -Werror -g src/vectorRendering.cpp -c

.PHONY: clean

clean:
	rm -f *.o core* asteroids valgrind*
