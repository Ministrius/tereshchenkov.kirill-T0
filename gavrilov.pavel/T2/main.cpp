#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <cmath>
#include <regex>

struct DataStruct {
    double key1;
    std::pair<long long, unsigned long long> key2;
    std::string key3;
};

bool parseDoubleLit(const std::string& str, double& value) {
    std::regex litRegex("^([0-9]+\\.[0-9]+)[dD]$");
    std::smatch match;
    if (std::regex_match(str, match, litRegex)) {
        try {
            value = std::stod(match[1].str());
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool parseRational(const std::string& str, std::pair<long long, unsigned long long>& value) {
    std::regex ratRegex("^\\(:N\\s+(-?[0-9]+):D\\s+([0-9]+):\\)$");
    std::smatch match;
    if (std::regex_match(str, match, ratRegex)) {
        try {
            long long numerator = std::stoll(match[1].str());
            unsigned long long denominator = std::stoull(match[2].str());
            if (denominator == 0) return false;
            value = std::make_pair(numerator, denominator);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool parseQuotedString(const std::string& str, std::string& value) {
    std::regex stringRegex("^\"([^\"]*)\"$");
    std::smatch match;
    if (std::regex_match(str, match, stringRegex)) {
        value = match[1].str();
        return true;
    }
    return false;
}

bool parseDataStruct(const std::string& line, DataStruct& data) {
    if (line.empty() || line.front() != '(' || line.back() != ')') {
        return false;
    }

    std::string content = line.substr(1, line.length() - 2);

    std::vector<std::string> parts;
    std::string current;
    int bracketDepth = 0;
    bool inQuotes = false;

    for (char c : content) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current += c;
        } else if (c == '(' && !inQuotes) {
            bracketDepth++;
            current += c;
        } else if (c == ')' && !inQuotes) {
            bracketDepth--;
            current += c;
        } else if (c == ':' && bracketDepth == 0 && !inQuotes) {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }

    double key1 = 0.0;
    std::pair<long long, unsigned long long> key2;
    std::string key3;
    bool key1Set = false;
    bool key2Set = false;
    bool key3Set = false;

    for (const auto& part : parts) {
        size_t spacePos = part.find(' ');
        if (spacePos == std::string::npos) {
            return false;
        }

        std::string fieldName = part.substr(0, spacePos);
        std::string fieldValue = part.substr(spacePos + 1);

        if (fieldName == "key1") {
            if (parseDoubleLit(fieldValue, key1)) {
                key1Set = true;
            } else {
                return false;
            }
        } else if (fieldName == "key2") {
            if (parseRational(fieldValue, key2)) {
                key2Set = true;
            } else {
                return false;
            }
        } else if (fieldName == "key3") {
            if (parseQuotedString(fieldValue, key3)) {
                key3Set = true;
            } else {
                return false;
            }
        }
    }

    if (!key1Set || !key2Set || !key3Set) {
        return false;
    }

    data.key1 = key1;
    data.key2 = key2;
    data.key3 = key3;
    return true;
}

std::string formatDoubleLit(double value) {
    std::ostringstream oss;
    oss.precision(1);
    oss << std::fixed << value << "d";
    return oss.str();
}

std::string formatRational(const std::pair<long long, unsigned long long>& value) {
    std::ostringstream oss;
    oss << "(:N " << value.first << ":D " << value.second << ":)";
    return oss.str();
}

std::string formatQuotedString(const std::string& value) {
    return "\"" + value + "\"";
}

std::ostream& operator<<(std::ostream& out, const DataStruct& data) {
    out << "(:key1 " << formatDoubleLit(data.key1)
        << ":key2 " << formatRational(data.key2)
        << ":key3 " << formatQuotedString(data.key3) << ":)";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) {
        return a.key1 < b.key1;
    }
    double aKey2Value = static_cast<double>(a.key2.first) / a.key2.second;
    double bKey2Value = static_cast<double>(b.key2.first) / b.key2.second;
    if (std::abs(aKey2Value - bKey2Value) > 1e-12) {
        return aKey2Value < bKey2Value;
    }
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> dataVector;
    std::string line;

    while (std::getline(std::cin, line)) {
        DataStruct data;
        if (parseDataStruct(line, data)) {
            dataVector.push_back(data);
        }
    }

    std::sort(dataVector.begin(), dataVector.end(), compareDataStruct);

    for (const auto& data : dataVector) {
        std::cout << data << "\n";
    }

    return 0;
}
