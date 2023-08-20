#include "utils.hpp"

#include <iostream>

namespace utils {

#define BIT(x, i) (((x) >> (i)) & 1)

    char* unicodeToChar(int unicode) {
        char* c;

        if (unicode < 128) {
            c = new char[2];
            c[0] = unicode;
            c[1] = '\0';
        } else if (unicode < 2048) {
            c = new char[3];
            c[0] = 192 | (unicode >> 6);
            c[1] = 128 | (unicode & 63);
            c[2] = '\0';
        } else if (unicode < 65536) {
            c = new char[4];
            c[0] = 224 | (unicode >> 12);
            c[1] = 128 | ((unicode >> 6) & 63);
            c[2] = 128 | (unicode & 63);
            c[3] = '\0';
        } else {
            c = new char[5];
            c[0] = 240 | (unicode >> 18);
            c[1] = 128 | ((unicode >> 12) & 63);
            c[2] = 128 | ((unicode >> 6) & 63);
            c[3] = 128 | (unicode & 63);
            c[4] = '\0';
        }

        return c;
    }

}  // namespace utils