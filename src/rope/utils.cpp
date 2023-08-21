#include "rope/utils.hpp"

#include <array>
#include <cstddef>

#include "rope/rope.hpp"

const std::array< std::size_t, Rope::maxDepth > make_fibs() {
    std::array< std::size_t, Rope::maxDepth > result{};
    result[0] = 1;
    result[1] = 1;
    for (std::size_t i = 2; i < Rope::maxDepth; ++i) {
        result[i] = result[i - 1] + result[i - 2];
    }
    return result;
}

const std::array< std::size_t, Rope::maxDepth > fibs = make_fibs();

std::size_t fib(std::size_t n) { return fibs[n]; }

const std::array< std::size_t, Rope::maxDepth > fib_list() { return fibs; }