#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Utils {
    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[Error] Failed to open file: " << path << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string getResourcePath(const std::string& relativePath) {
#ifdef PROJECT_ROOT_DIR
        return std::string(PROJECT_ROOT_DIR) + "/" + relativePath;
#else
        return relativePath;
#endif
    }
}
