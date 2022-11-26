/**
 * @file test.hpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Test tools class declaration
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#pragma once

#include <memory>
#include <stdexcept>
#include <vector>
#include "randres.hpp"

class Exception : public std::exception {
public:
    Exception(const string & _msg) noexcept : msg(_msg) { }
    const char* what() const noexcept { return msg.c_str(); }
private:
    string msg;
};

class Test {
public:
    Test(int argc, char ** argv);
    void run();
    static void print_help(const char * argv0);

private:
    long size;
    std::vector<std::unique_ptr<RandomResource>> items;

    long measure_time(RandomResource & random);
    void print_performance(RandomResource & random, long micros);
};
