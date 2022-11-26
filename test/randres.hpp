/**
 * @file randres.hpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Random resource reader library
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#pragma once

#include <ctime>
#include <fstream>
#include <random>
#include <string>

using std::string;

#define LFSR_DEVICE_PATH "/dev/lfsr"
#define URANDOM_DEVICE_PATH "/dev/urandom"

class RandomResource {
public:
    RandomResource(const string & _name) : name(_name) { }
    virtual ~RandomResource() { };
    virtual void get(char * buffer, size_t n) = 0;
    const string & get_name() const { return name; };

protected:
    string name;
};

class CRandom : public RandomResource {
public:
    CRandom() : RandomResource("CRandom"), seed(time(NULL)) { }
    ~CRandom() { }
    void get(char * buffer, size_t n);

private:
    unsigned int seed;
};

class Mersenne : public RandomResource {
public:
    Mersenne() : RandomResource("Mersenne") { }
    void get(char * buffer, size_t n);

private:
    std::mt19937_64 twister;
};

class RandomFile : public RandomResource {
public:
    RandomFile(const string & name, const string & filename);
    RandomFile(RandomFile && other);
    ~RandomFile();
    void get(char * buffer, size_t n);

private:
    std::ifstream file;
};

class URandom : public RandomFile {
public:
    URandom() : RandomFile("URandom", URANDOM_DEVICE_PATH) { }
};

class LFSR : public RandomFile {
public:
    LFSR() : RandomFile("LFSR", LFSR_DEVICE_PATH) { }
};
