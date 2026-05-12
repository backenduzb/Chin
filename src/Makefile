CC = g++

CFLAGS = -Wall -std=c++17 \
-I../external/tinygltf \
-I../include \
-MMD -MP

DIST_DIR = dist
DEP_DIR = depends

SRCS = $(wildcard *.cpp)

OBJS = $(patsubst %.cpp,$(DIST_DIR)/%.o,$(SRCS))
DEPS = $(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRCS))

all: dirs src.o

dirs:
	mkdir -p $(DIST_DIR)
	mkdir -p $(DEP_DIR)

$(DIST_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/$*.d -c $< -o $@

$(DIST_DIR)/glad.o: glad.c
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/glad.d -c glad.c -o $@

src.o: $(OBJS) $(DIST_DIR)/glad.o
	ld -r -o src.o $(OBJS) $(DIST_DIR)/glad.o

-include $(DEPS) $(DEP_DIR)/glad.d

clean:
	rm -rf $(DIST_DIR) $(DEP_DIR) src.o