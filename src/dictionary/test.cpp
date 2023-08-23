#include <iostream>

#include "dictionary/dictionary.hpp"

int main() {
    Dictionary dict;
    dict.loadDatabase("data/dictionary/english/words.txt");

    std::cout << dict.search("hello") << std::endl;

    std::cout << dict.search("world") << std::endl;

    std::vector< nstring > suggestions = dict.suggest("helo");

    std::size_t cnt = 0;

    for (const auto& suggestion : suggestions) {
        if (cnt < 100) std::cout << suggestion << std::endl;
        cnt++;
    }

    return 0;
}