#include <iostream>

#include "dictionary/dictionary.hpp"

int main() {
    Dictionary dict;
    dict.loadDatabase("data/dictionary/english/words.txt");

    std::cout << dict.search("hello") << std::endl;

    std::cout << dict.search("world") << std::endl;

    return 0;
}