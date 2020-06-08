.DEFAULT_GOAL := scratch
scratch:
	@echo "Compiling all files"
	mkdir build
	mkdir log
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread
	gcc ./test/test.c -o ./build/test -I ./include/ -lpthread
compile:
	@echo "Compiling all files"
	gcc ./src/client.c -o ./build/client -I ./include/ -lpthread
	gcc ./src/server.c -o ./build/server -I ./include/ -lpthread
	gcc ./test/test.c -o ./build/test -I ./include/ -lpthread
clean:
	rm -rf log build log_temp