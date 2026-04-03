#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <complex>
#include <iomanip>
#include <sstream>

struct DataStruct {
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;
};

bool isHexNumber(const std::string& s, unsigned long long& result) {
    if (s.length() < 3) return false;

    if (s[0] != '0') return false;
    if (s[1] != 'x' && s[1] != 'X') return false;

    for (size_t i = 2; i < s.length(); i++) {
        char c = s[i];
        bool isDigit = (c >= '0' && c <= '9');
        bool isHex = (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!isDigit && !isHex) return false;
    }

    std::stringstream ss(s);
    ss >> std::hex >> result;
    return !ss.fail();
}

bool isComplexNumber(const std::string& s, std::complex<double>& result) {
    if (s.length() < 7) return false;

    if (s[0] != '#' || s[1] != 'c' || s[2] != '(') return false;
    if (s.back() != ')') return false;

    std::string inner = s.substr(3, s.length() - 4);

    size_t spacePos = inner.find(' ');
    if (spacePos == std::string::npos) return false;
    if (spacePos == 0 || spacePos == inner.length() - 1) return false;

    for (size_t i = 0; i < inner.length(); i++) {
        if (i != spacePos && inner[i] == ' ') return false;
    }

    std::string realStr = inner.substr(0, spacePos);
    std::string imagStr = inner.substr(spacePos + 1);

    double real, imag;
    std::stringstream ss1(realStr), ss2(imagStr);

    if (!(ss1 >> real) || !(ss2 >> imag)) return false;

    char extra;
    if (ss1 >> extra || ss2 >> extra) return false;

    result = std::complex<double>(real, imag);
    return true;
}

bool isQuotedString(const std::string& s, std::string& result) {
    if (s.length() < 2) return false;

    if (s.front() != '"' || s.back() != '"') return false;

    result = s.substr(1, s.length() - 2);
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& data) {
    std::string line;

    if (!std::getline(in, line)) {
        return in;
    }

    size_t start = line.find('(');
    size_t end = line.rfind(')');

    if (start == std::string::npos || end == std::string::npos || end <= start) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string content = line.substr(start + 1, end - start - 1);

    unsigned long long key1Val = 0;
    std::complex<double> key2Val(0, 0);
    std::string key3Val;

    bool found1 = false, found2 = false, found3 = false;

    std::vector<std::string> parts;
    bool inQuotes = false;
    bool inComplex = false;
    size_t lastPos = 0;

    for (size_t i = 0; i < content.length(); i++) {
        if (content[i] == '"') {
            inQuotes = !inQuotes;
        }
        else if (!inQuotes && content[i] == '#') {
            inComplex = true;
        }
        else if (!inQuotes && content[i] == ')' && inComplex) {
            inComplex = false;
        }
        else if (!inQuotes && !inComplex && content[i] == ':') {
            if (i > lastPos) {
                parts.push_back(content.substr(lastPos, i - lastPos));
            }
            lastPos = i + 1;
        }
    }
    if (lastPos < content.length()) {
        parts.push_back(content.substr(lastPos));
    }

    for (const std::string& part : parts) {
        if (part.empty()) continue;

        size_t spacePos = part.find(' ');
        if (spacePos == std::string::npos) continue;

        std::string name = part.substr(0, spacePos);
        std::string value = part.substr(spacePos + 1);

        if (name == "key1") {
            if (isHexNumber(value, key1Val)) {
                found1 = true;
            }
        }
        else if (name == "key2") {
            if (isComplexNumber(value, key2Val)) {
                found2 = true;
            }
        }
        else if (name == "key3") {
            if (isQuotedString(value, key3Val)) {
                found3 = true;
            }
        }
    }

    if (found1 && found2 && found3) {
        data.key1 = key1Val;
        data.key2 = key2Val;
        data.key3 = key3Val;
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& data) {

    out << "(:key1 0x" << std::hex << std::uppercase << data.key1;
    out << std::dec << std::nouppercase;

    out << ":key2 #c(" << std::fixed << std::setprecision(1);
    out << data.key2.real() << " " << data.key2.imag();
    out << std::defaultfloat << ")";

    out << ":key3 \"" << data.key3 << "\":)";

    return out;
}

bool compareData(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) {
        return a.key1 < b.key1;
    }

    double modA = std::abs(a.key2);
    double modB = std::abs(b.key2);
    if (modA != modB) {
        return modA < modB;
    }

    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> records;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        if (line.find('(') == std::string::npos) continue;

        std::istringstream iss(line);
        DataStruct temp;

        if (iss >> temp) {
            records.push_back(temp);
        }
    }

    if (records.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }

    std::sort(records.begin(), records.end(), compareData);

    std::copy(
        records.begin(),
        records.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
