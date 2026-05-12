.PHONY: all src utils build run clean

CC = g++

LIBS = -lglfw -ldl -lGL

all: build

src:
	$(MAKE) -C src

utils:
	$(MAKE) -C utils

	
build: src utils
	$(CC) src/src.o utils/utils.o -o app $(LIBS)

run: build
	./app

clean:
	rm -rf app
	$(MAKE) -C src clean
	$(MAKE) -C utils clean