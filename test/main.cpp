/**
 * @file main.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Random resources performance test
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include <iostream>
#include <chrono>
#include "randres.hpp"
#include "test.hpp"

#define GET_SIZE    1073741824  // 2^30 B = 1 GiB
#define BUFFER_SIZE 16384       // 2^14 B = 16 KiB

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv) {
    try {
        auto test = Test(argc, argv);
        test.run();
    } catch (invalid_argument & e) {
        cerr << "ERROR: " << e.what() << endl;
        Test::print_help(argv[0]);
        return EXIT_FAILURE;
    } catch (exception & e) {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
