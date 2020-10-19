all: run

build: guess_server.c
	gcc guess_server.c -o guess_server

run: build
	./guess_server

run_valgrind: build
	valgrind ./guess_server

clean:
	rm ./guess_server
