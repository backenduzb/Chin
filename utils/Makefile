CC = g++

CFLAGS = -Wall -std=c++17 \
-I../external/tinygltf \
-I../include

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: utils.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
utils.o: $(OBJS)
	ld -r -o utils.o $(OBJS)

clean:
	rm -f **.o