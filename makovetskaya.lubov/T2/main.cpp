#include "DataStruct.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

bool cmp(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    if (a.key3.size() != b.key3.size()) {
        return a.key3.size() < b.key3.size();
    }
    return a.key3 < b.key3;
}

int main() {
    std::vector< DataStruct > data;
    DataStruct tmp;
    while (!std::cin.eof()) {
        if (std::cin >> tmp) {
            data.push_back(tmp);
        }
        else {
            if (std::cin.eof()) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(1);
        }
    }

    std::sort(std::begin(data), std::end(data), cmp);
    std::copy(
        std::begin(data),
        std::end(data),
        std::ostream_iterator< DataStruct >(std::cout, "\n")
    );
    return 0;
}
