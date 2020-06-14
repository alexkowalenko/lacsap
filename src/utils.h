#pragma once

#include <algorithm>
#include <string>

inline void strlower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string GetPath(const std::string &fileName);
