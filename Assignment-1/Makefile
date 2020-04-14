.DEFAULT_GOAL := compile
compile:
	@echo "Compiling"
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread
clean:
	rm ./build/client
	rm ./build/server
	rm ./log/*
