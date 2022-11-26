/**
 * @file test.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Test tools class definition
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <climits>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include "test.hpp"

#define DEFAULT_SIZE    1073741824  // 2^30 B = 1 GiB
#define BUFFER_SIZE     16384       // 2^14 B = 16 KiB

using namespace std;
using namespace chrono;

Test::Test(int argc, char ** argv) : size(DEFAULT_SIZE) {
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "hs:")) != -1) {
        switch (c) {
        case 'h':
            print_help(argv[0]);
            exit(EXIT_SUCCESS);

        case 's':
            try {
                size = std::stol(optarg);
            } catch (logic_error &) {
                throw invalid_argument("Invalid size value.");
            }

            if (size < (int)sizeof(int)) {
                throw invalid_argument("Invalid size value.");
            }

            break;

        case '?':
            throw invalid_argument("Invalid option.");
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strncmp("crandom", argv[i], strlen(argv[i])) == 0) {
            items.push_back(make_unique<CRandom>(CRandom()));
        } else if (strncmp("lfsr", argv[i], strlen(argv[i])) == 0) {
            items.push_back(make_unique<LFSR>(LFSR()));
        } else if (strncmp("mersenne", argv[i], strlen(argv[i])) == 0) {
            items.push_back(make_unique<Mersenne>(Mersenne()));
        } else if (strncmp("urandom", argv[i], strlen(argv[i])) == 0) {
            items.push_back(make_unique<URandom>(URandom()));
        } else {
            cerr << "WARN: Unknown generator: " << argv[i] << ".\n";
        }
    }

    if (items.size() == 0) {
        items.push_back(make_unique<CRandom>(CRandom()));
        items.push_back(make_unique<LFSR>(LFSR()));
        items.push_back(make_unique<Mersenne>(Mersenne()));
        items.push_back(make_unique<URandom>(URandom()));
    }
}

void Test::run() {
    for (auto it = items.begin(); it != items.end(); ++it) {
        long micros = measure_time(**it);
        print_performance(**it, micros);
    }
}

void Test::print_help(const char * argv0) {
    cout << "Syntax: " << argv0 << " [-h] [-s SIZE] [ GEN ... ]\n"
            "  -h       Show this help.\n"
            "  -s SIZE  Data length to get from the generators (default: " << DEFAULT_SIZE << ")\n"
            "\n"
            "  Generators:\n"
            "    - CRandom: C linear congruential generator\n"
            "    - LFSR: /dev/lfsr\n"
            "    - Mersenne: MT19937 Mersenne Twister\n"
            "    - URandom: /dev/urandom\n";
}

long Test::measure_time(RandomResource & random) {
    char buffer[BUFFER_SIZE];
    size_t length;
    auto begin = std::chrono::steady_clock::now();

    for (length = size; length >= BUFFER_SIZE; length -= BUFFER_SIZE) {
        random.get(buffer, BUFFER_SIZE);
    }

    if (length > 0) {
        random.get(buffer, length);
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = duration_cast<chrono::microseconds>(end - begin);

    return duration.count();
}

void Test::print_performance(RandomResource & random, long micros) {
    cout << "- " << random.get_name() << ":\n";
    cout << "  - Time:        " << micros / 1e3 << " ms.\n";
    cout << "  - Performance: " << (double)size / micros << " MB/s.\n";
}
