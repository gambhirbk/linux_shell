all: run

build: guess_client.c
	gcc guess_client.c -o guess_client

run: build
	./guess_client

run_valgrind: build
	valgrind ./guess_client

clean:
	rm guess_client
