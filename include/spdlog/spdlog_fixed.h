// Stub spdlog header for basic compilation
#pragma once

#include <iostream>
#include <string>

namespace spdlog {
// Global functions
template <typename... Args> void info(const std::string &fmt, Args &&...args) {
  std::cout << "[INFO] " << fmt << std::endl;
}

template <typename... Args> void warn(const std::string &fmt, Args &&...args) {
  std::cout << "[WARN] " << fmt << std::endl;
}

template <typename... Args> void error(const std::string &fmt, Args &&...args) {
  std::cout << "[ERROR] " << fmt << std::endl;
}
} // namespace spdlog