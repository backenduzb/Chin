#pragma once
#include <string>

namespace Utils {
    std::string readFile(const std::string& path);
    std::string getResourcePath(const std::string& relativePath);
}
