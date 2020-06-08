.DEFAULT_GOAL := scratch
scratch:
	@echo "Compiling all files"
	mkdir -p build
	mkdir -p log
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread -lpulse -lpulse-simple
compile:
	@echo "Compiling all files"
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread -lpulse -lpulse-simple
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread -lpulse -lpulse-simple
clean:
	rm -rf log build