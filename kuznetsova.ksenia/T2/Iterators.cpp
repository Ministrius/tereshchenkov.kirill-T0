#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <limits>
#include <complex>
#include <sstream>
#include <cctype>

namespace nspace {
    const double EPS = 1e-9;

    struct DataStruct {
        unsigned long long key1 = 0;
        std::complex<double> key2{ 0.0, 0.0 };
        std::string key3;
    };

    // Функция для парсинга числа с суффиксом
    bool parseNumber(const std::string& token, unsigned long long& dest) {
        if (token.empty()) return false;

        std::string numStr = token;

        // Убираем суффикс
        if (token.back() == 'd') {
            numStr = token.substr(0, token.length() - 1);
        }
        else if (token.length() >= 2 && token.substr(token.length() - 2) == "ll") {
            numStr = token.substr(0, token.length() - 2);
        }
        else if (token.length() >= 3 && token.substr(token.length() - 3) == "ull") {
            numStr = token.substr(0, token.length() - 3);
        }
        else if (token.length() >= 2 && token.substr(0, 2) == "0x") {
            // hex
            std::istringstream iss(token);
            iss >> std::hex >> dest;
            return !iss.fail();
        }
        else if (token.length() >= 2 && token.substr(0, 2) == "0b") {
            // binary
            numStr = token.substr(2);
            dest = 0;
            for (char c : numStr) {
                if (c == '0') dest = dest * 2;
                else if (c == '1') dest = dest * 2 + 1;
                else return false;
            }
            return true;
        }

        // Обычное число
        std::istringstream iss(numStr);
        iss >> dest;
        return !iss.fail();
    }

    // Функция для парсинга double с суффиксом
    bool parseDouble(const std::string& token, double& dest) {
        if (token.empty()) return false;

        std::string numStr = token;
        if (token.back() == 'd') {
            numStr = token.substr(0, token.length() - 1);
        }

        std::istringstream iss(numStr);
        iss >> dest;
        return !iss.fail();
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;

        // Пропускаем пробелы
        while (std::isspace(in.peek())) in.get();

        // Читаем '('
        if (in.peek() != '(') {
            in.setstate(std::ios::failbit);
            return in;
        }
        in.get();

        DataStruct input;
        bool hasKey1 = false, hasKey2 = false, hasKey3 = false;

        // Читаем три пары
        for (int i = 0; i < 3; i++) {
            // Пропускаем пробелы
            while (std::isspace(in.peek())) in.get();

            // Читаем ':'
            if (in.peek() != ':') {
                in.setstate(std::ios::failbit);
                return in;
            }
            in.get();

            // Читаем ключ (key1, key2 или key3)
            std::string key;
            while (in.peek() && std::isalpha(in.peek())) {
                key += in.get();
            }

            // Пропускаем пробелы перед ':'
            while (std::isspace(in.peek())) in.get();

            // Читаем второй ':'
            if (in.peek() != ':') {
                in.setstate(std::ios::failbit);
                return in;
            }
            in.get();

            // Пропускаем пробелы перед значением
            while (std::isspace(in.peek())) in.get();

            // Читаем значение в зависимости от ключа
            if (key == "key1") {
                // Читаем число
                std::string value;
                while (in.peek() && (std::isalnum(in.peek()) || in.peek() == 'x' || in.peek() == 'b' || in.peek() == '.' || in.peek() == '-')) {
                    value += in.get();
                }
                if (!parseNumber(value, input.key1)) {
                    in.setstate(std::ios::failbit);
                    return in;
                }
                hasKey1 = true;
            }
            else if (key == "key2") {
                // Проверяем, не комплексное ли число
                if (in.peek() == '#') {
                    in.get(); // '#'
                    if (in.get() != 'c') {
                        in.setstate(std::ios::failbit);
                        return in;
                    }
                    if (in.get() != '(') {
                        in.setstate(std::ios::failbit);
                        return in;
                    }
                    double real, imag;
                    in >> real >> imag;
                    if (in.get() != ')') {
                        in.setstate(std::ios::failbit);
                        return in;
                    }
                    input.key2 = std::complex<double>(real, imag);
                }
                else {
                    // Обычное число
                    std::string value;
                    while (in.peek() && (std::isalnum(in.peek()) || in.peek() == '.' || in.peek() == '-' || in.peek() == 'e' || in.peek() == 'E')) {
                        value += in.get();
                    }
                    double num;
                    if (!parseDouble(value, num)) {
                        in.setstate(std::ios::failbit);
                        return in;
                    }
                    input.key2 = std::complex<double>(num, 0.0);
                }
                hasKey2 = true;
            }
            else if (key == "key3") {
                // Читаем строку в кавычках
                if (in.get() != '"') {
                    in.setstate(std::ios::failbit);
                    return in;
                }
                std::getline(in, input.key3, '"');
                hasKey3 = true;
            }
            else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }

        // Читаем ')'
        while (std::isspace(in.peek())) in.get();
        if (in.get() != ')') {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (hasKey1 && hasKey2 && hasKey3) {
            dest = input;
        }
        else {
            in.setstate(std::ios::failbit);
        }

        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
        out << "(:key1 " << src.key1;

        // Проверяем, целое ли число у key2
        if (std::abs(src.key2.imag()) < EPS) {
            out << ":key2 " << static_cast<long long>(src.key2.real()) << "d";
        }
        else {
            out << ":key2 #c(" << std::fixed << std::setprecision(1)
                << src.key2.real() << " " << src.key2.imag() << ")";
        }

        out << ":key3 \"" << src.key3 << "\":)";

        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) {
            return a.key1 < b.key1;
        }

        double modA = std::abs(a.key2);
        double modB = std::abs(b.key2);
        if (std::abs(modA - modB) > EPS) {
            return modA < modB;
        }

        return a.key3.length() < b.key3.length();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;
    nspace::DataStruct temp;

    while (std::cin >> temp) {
        data.push_back(temp);
    }

    if (data.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 1;
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    for (const auto& item : data) {
        std::cout << item << "\n";
    }

    return 0;
}
