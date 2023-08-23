#include <iostream>

#include "search/search.hpp"

void testSearch(std::string text, std::string pattern) {
    Rope rope(text);
    Search search;
    search.set_pattern(Rope(pattern));
    search.find_in_content(rope);
    std::cout << "Input:    " << text << std::endl;
    std::cout << "Matches (" << search.match_idx().size() << "): " << std::endl;
    for (auto& match_idx : search.match_idx()) {
        std::cout << match_idx << " ";
    }
    std::cout << std::endl;
}

void testReplace(std::string text, std::string pattern,
                 std::string replacement) {
    Rope rope(text);
    Search search;
    search.set_pattern(Rope(pattern));
    search.set_replacement(Rope(replacement));
    Rope result = search.replace_in_content(rope);
    std::cout << "Input:    " << text << std::endl;
    std::cout << "Result:   " << result << std::endl;
}

int main() {
    testSearch("abc", "a");
    testSearch("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
    testSearch("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaa");
    testSearch("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaa");
    testSearch("word1 word2 word3", "word");

    testReplace("abc", "a", "b");
    testReplace("abc", "a", "bc");
    testReplace("abc", "a", "bcd");
    testReplace("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaa", "b");

    testSearch(
        "xin chào các bạn mình là Lộc, đến từ trường Khoa Học Tự Nhiên - ĐHQG, "
        "TP.HCM",
        "Lộc");

    return 0;
}