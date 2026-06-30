.PHONY: all build run clean

all: build

build:
	mkdir -p build
	cd build && cmake .. && cmake --build .
	cp build/app .

run: build
	./app

clean:
	rm -rf build app