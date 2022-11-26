/**
 * @file crandom.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief C Random resource reader implementation
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <cstring>
#include "randres.hpp"

void CRandom::get(char * buffer, size_t n) {
    while (n >= sizeof(int)) {
        rand_r(&seed);
        memcpy(buffer, &seed, sizeof(int));
        n -= sizeof(int);
        buffer += sizeof(int);
    }
}
