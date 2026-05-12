#include "../include/Reader.h"

#include <fstream>
#include <sstream>

std::string readFile(const char* path) {

    std::ifstream file(path);

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}