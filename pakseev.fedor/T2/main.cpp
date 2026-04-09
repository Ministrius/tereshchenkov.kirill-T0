#include "DataStruct.hpp"

#include <algorithm>
#include <vector>
#include <iterator>
#include <limits>

// --- 4 ---
int main() {
    std::vector<DataStruct> vec;

    //Changed way of getting lines: reading line by line with getline to prevent eof infitite loop
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        DataStruct temp;

        if (iss >> temp) {
            vec.push_back(temp);
        }
    }

    std::sort(vec.begin(), vec.end(), comparator);
    std::copy(
        std::begin(vec),
        std::end(vec),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
