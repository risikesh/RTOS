.DEFAULT_GOAL := all
all:
	@echo "Compiling all files"
	mkdir -p build
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread -lpulse -lpulse-simple
compile:
	@echo "Compiling all files"
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread -lpulse -lpulse-simple
clean:
	rm -rf build
