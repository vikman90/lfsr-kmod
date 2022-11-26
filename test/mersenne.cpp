/**
 * @file mersenne.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Mersenne Twister random reader implementation
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <cstring>
#include "randres.hpp"

void Mersenne::get(char * buffer, size_t n) {
    while (n >= sizeof(int)) {
        int value = twister();
        memcpy(buffer, &value, sizeof(int));
        n -= sizeof(int);
        buffer += sizeof(int);
    }
}
