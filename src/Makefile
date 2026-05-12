CC = g++

CFLAGS = -Wall -std=c++17 \
-I../external/tinygltf \
-I../include \
-MMD -MP

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: src.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

glad.o: glad.c
	$(CC) $(CFLAGS) -c glad.c -o glad.o

src.o: $(OBJS) glad.o
	ld -r -o src.o $(OBJS) glad.o

-include $(DEPS)

clean:
	rm -f *.o *.d src.o