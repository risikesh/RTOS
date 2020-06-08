DEFAULT_GOAL := all
all :
	@echo "Compiling all files"
	mkdir -p build
	gcc ./src/client_server.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple -lSDL2main -lSDL2 -lSDL2_image `sdl2-config --cflags --libs`
	gcc ./src/turn_server.c -o ./build/turn_server -I ./include/ -lpthread 
client :
	mkdir -p build
	gcc ./src/client_server.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple -lSDL2main -lSDL2 -lSDL2_image `sdl2-config --cflags --libs`
server :
	mkdir -p build
	gcc ./src/turn_server.c -o ./build/turn_server -I ./include/ -lpthread
clean :
	rm -rf build
	