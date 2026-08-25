#pragma once
#include <print>

namespace lg {
inline void info(std::string msg) {
    std::println("\e[1;32mINFO\e[0m: {}", msg);
}
inline void debug(std::string msg) {
    std::println("\e[1;34mDEBUG\e[0m: {}", msg);
}
inline void warn(std::string msg) {
    std::println("\e[1;33mWARN\e[0m: {}", msg);
}
inline void err(std::string msg) {
    std::println("\e[1;31mERROR\e[0m: {}", msg);
}
} // namespace lg
