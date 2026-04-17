#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <iomanip>
#include <complex>
#include <cmath>
#include <sstream>
#include <cctype>

namespace nspace {
    struct DataStruct {
        unsigned long long key1;
        std::complex<double> key2;
        std::string key3;
    };

    bool parseHexULL(const std::string& str, unsigned long long& value) {
        if (str.length() < 3) return false;
        if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) return false;
        if (str.length() == 2) return false;

        for (size_t i = 2; i < str.length(); ++i) {
            char c = str[i];
            if (!((c >= '0' && c <= '9') ||
                (c >= 'a' && c <= 'f') ||
                (c >= 'A' && c <= 'F'))) {
                return false;
            }
        }

        std::stringstream s(str);
        s >> std::hex >> value;
        return !s.fail();
    }

    bool parseComplex(const std::string& str, std::complex<double>& value) {
        if (str.length() < 6 || str[0] != '#' ||
            (str[1] != 'c' && str[1] != 'C') || str[2] != '(') {
            return false;
        }

        size_t close_pos = str.find(')');
        if (close_pos == std::string::npos || close_pos != str.length() - 1) {
            return false;
        }

        std::string inner = str.substr(3, close_pos - 3);
        std::stringstream ss(inner);
        double real, imag;

        if (!(ss >> real >> imag)) {
            return false;
        }

        char leftover;
        if (ss >> leftover) {
            return false;
        }

        size_t space_pos = inner.find(' ');
        if (space_pos == std::string::npos) {
            return false;
        }
        if (space_pos > 0 && inner[space_pos - 1] == ' ') {
            return false;
        }
        if (space_pos + 1 < inner.length() && inner[space_pos + 1] == ' ') {
            return false;
        }

        value = std::complex<double>(real, imag);
        return true;
    }

    bool parseString(const std::string& str, std::string& value) {
        if (str.length() < 2 || str.front() != '"' || str.back() != '"') {
            return false;
        }
        value = str.substr(1, str.length() - 2);
        return true;
    }

    std::istream& operator>>(std::istream& is, DataStruct& data) {
        std::string line;
        if (!std::getline(is, line)) {
            return is;
        }

        size_t start = line.find('(');
        if (start == std::string::npos) {
            is.setstate(std::ios::failbit);
            return is;
        }

        size_t end = line.rfind(')');
        if (end == std::string::npos || end <= start) {
            is.setstate(std::ios::failbit);
            return is;
        }

        std::string content = line.substr(start + 1, end - start - 1);

        unsigned long long key1 = 0;
        std::complex<double> key2(0, 0);
        std::string key3;
        bool key1_found = false, key2_found = false, key3_found = false;

        std::vector<std::string> parts;
        bool in_quotes = false;
        bool in_complex = false;
        size_t last_pos = 0;

        for (size_t i = 0; i < content.length(); ++i) {
            if (content[i] == '"') {
                in_quotes = !in_quotes;
            }
            else if (!in_quotes && content[i] == '#') {
                in_complex = true;
            }
            else if (!in_quotes && content[i] == ')' && in_complex) {
                in_complex = false;
            }
            else if (!in_quotes && !in_complex && content[i] == ':') {
                if (i > last_pos) {
                    parts.push_back(content.substr(last_pos, i - last_pos));
                }
                last_pos = i + 1;
            }
        }
        if (last_pos < content.length()) {
            parts.push_back(content.substr(last_pos));
        }

        for (const auto& part : parts) {
            if (part.empty()) continue;

            size_t space_pos = part.find(' ');
            if (space_pos == std::string::npos) continue;

            std::string name = part.substr(0, space_pos);
            std::string value_str = part.substr(space_pos + 1);

            if (name == "key1") {
                if (parseHexULL(value_str, key1)) {
                    key1_found = true;
                }
            }
            else if (name == "key2") {
                if (parseComplex(value_str, key2)) {
                    key2_found = true;
                }
            }
            else if (name == "key3") {
                if (parseString(value_str, key3)) {
                    key3_found = true;
                }
            }
        }

        if (key1_found && key2_found && key3_found) {
            data.key1 = key1;
            data.key2 = key2;
            data.key3 = key3;
            return is;
        }

        is.setstate(std::ios::failbit);
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const DataStruct& data) {
        os << "(:key1 0x" << std::hex << std::uppercase << data.key1
            << std::dec << std::nouppercase;
        os << ":key2 #c(";
        os << std::fixed << std::setprecision(1);
        os << data.key2.real() << " " << data.key2.imag();
        os << std::defaultfloat << ")";
        os << ":key3 \"" << data.key3 << "\":)";
        return os;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) {
            return a.key1 < b.key1;
        }
        double ma = std::abs(a.key2);
        double mb = std::abs(b.key2);
        if (ma != mb) {
            return ma < mb;
        }
        return a.key3.length() < b.key3.length();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        nspace::DataStruct temp;
        if (iss >> temp) {
            data.push_back(temp);
        }
    }

    if (data.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<nspace::DataStruct>(std::cout, "\n")
    );

    return 0;
}
