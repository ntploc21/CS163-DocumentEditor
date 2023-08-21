#ifndef ROPE_UTILS_HPP
#define ROPE_UTILS_HPP

#include <array>
#include <cstddef>

#include "rope/rope.hpp"

std::size_t fib(std::size_t n);

const std::array< std::size_t, Rope::maxDepth > fib_list();

#endif  // ROPE_UTILS_HPP