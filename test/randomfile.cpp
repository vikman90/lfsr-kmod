/**
 * @file randomfile.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Random file resource readerimplementation
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 Victor Manuel Fernandez Castro
 *
 */

#include "randres.hpp"

using namespace std;

RandomFile::RandomFile(const string & name, const string & filename) : RandomResource(name) {
    file.open(filename);

    if (!file) {
        throw runtime_error("Cannot open " + filename + ".");
    }
}

RandomFile::RandomFile(RandomFile && other) : RandomResource(other.name), file(std::move(other.file)) { }

RandomFile::~RandomFile() {
    file.close();
}

void RandomFile::get(char * buffer, size_t n) {
    file.read(buffer, n);
}
