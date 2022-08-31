#pragma once
#include <string>
#include <sstream>

namespace ZuneCraft {
    template <typename T> 
    std::string ToString(const T& n) {
        std::ostringstream stream;
        stream << n;
        return stream.str();
    }
}