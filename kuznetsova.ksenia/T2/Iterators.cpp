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

namespace nspace {
    const double EPS = 1e-9;

    struct DataStruct {
        double key1_dbl = 0;
        long long key1_sll = 0;
        unsigned long long key1_ull = 0;
        char key1_chr = 0;
        std::complex<double> key1_cmp{ 0, 0 };
        int key1_type = 0;

        double key2_dbl = 0;
        long long key2_sll = 0;
        unsigned long long key2_ull = 0;
        char key2_chr = 0;
        std::complex<double> key2_cmp{ 0, 0 };
        std::pair<long long, unsigned long long> key2_rat{ 0, 1 };
        int key2_type = 0;

        std::string key3;

        static int detected_variant;
    };

    int DataStruct::detected_variant = 0;

    struct DelimiterIO { char exp; };
    struct KeyIO { int& ref; };

    std::istream& operator>>(std::istream& in, KeyIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char k, e, y, n;
        in >> k >> e >> y >> n;
        if (in && k == 'k' && e == 'e' && y == 'y' && n >= '1' && n <= '3') {
            dest.ref = n - '0';
        }
        else {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char ch;
        in >> ch;
        if (in && std::tolower(ch) != std::tolower(dest.exp)) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    bool readDblLit(std::istream& in, double& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        std::string num;
        char ch;
        while (in.get(ch) && ch != 'd' && ch != 'D') {
            if (ch == ':') { in.setstate(std::ios::failbit); return false; }
            num += ch;
        }

        auto dot = num.find('.');
        if (dot == std::string::npos || dot == 0 || dot == num.size() - 1) {
            in.setstate(std::ios::failbit);
            return false;
        }

        try {
            val = std::stod(num);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readDblSci(std::istream& in, double& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        std::string num;
        char ch;
        while (in.get(ch) && ch != ':' && !std::isspace(ch)) {
            num += ch;
        }

        auto e_pos = num.find_first_of("eE");
        if (e_pos == std::string::npos) {
            in.setstate(std::ios::failbit);
            return false;
        }

        try {
            val = std::stod(num);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }
        return true;
    }

    bool readSllLit(std::istream& in, long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        std::string num;
        char ch;
        while (in.get(ch) && ch != 'l' && ch != 'L') {
            num += ch;
        }
        in >> ch;

        try {
            val = std::stoll(num);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readUllLit(std::istream& in, unsigned long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        std::string num;
        char ch;
        while (in.get(ch) && ch != 'u' && ch != 'U') {
            num += ch;
        }
        in >> ch >> ch;

        try {
            val = std::stoull(num);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readUllOct(std::istream& in, unsigned long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char c0, c1;
        in >> c0 >> c1;
        if (c0 != '0' || (c1 < '0' || c1 > '7')) {
            in.setstate(std::ios::failbit);
            return false;
        }

        std::string num;
        num += c1;
        while (in.get(c1) && c1 >= '0' && c1 <= '7') {
            num += c1;
        }
        in.putback(c1);

        try {
            val = std::stoull(num, nullptr, 8);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readUllBin(std::istream& in, unsigned long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char c0, c1;
        in >> c0 >> c1;
        if (c0 != '0' || (c1 != 'b' && c1 != 'B')) {
            in.setstate(std::ios::failbit);
            return false;
        }

        std::string num;
        while (in.get(c1) && (c1 == '0' || c1 == '1')) {
            num += c1;
        }
        in.putback(c1);

        try {
            val = std::stoull(num, nullptr, 2);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return false;
        }

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readUllHex(std::istream& in, unsigned long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char c0, c1;
        in >> c0 >> c1;
        if (c0 != '0' || (c1 != 'x' && c1 != 'X')) {
            in.setstate(std::ios::failbit);
            return false;
        }

        if (!(in >> std::hex >> val)) {
            in.setstate(std::ios::failbit);
            return false;
        }
        in >> std::dec;

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readChrLit(std::istream& in, char& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char q;
        in >> q;
        if (q != '\'') {
            in.setstate(std::ios::failbit);
            return false;
        }

        in.get(val);

        char q2, colon;
        in >> q2 >> colon;
        if (q2 != '\'' || colon != ':') {
            in.setstate(std::ios::failbit);
            return false;
        }
        return true;
    }

    bool readCmpLsp(std::istream& in, std::complex<double>& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char hash, c, open, close, colon;
        double real, imag;

        in >> hash >> c >> open;
        if (hash != '#' || (c != 'c' && c != 'C') || open != '(') {
            in.setstate(std::ios::failbit);
            return false;
        }

        if (!(in >> real >> imag)) {
            in.setstate(std::ios::failbit);
            return false;
        }

        in >> close >> colon;
        if (close != ')' || colon != ':') {
            in.setstate(std::ios::failbit);
            return false;
        }

        val = std::complex<double>(real, imag);
        return true;
    }

    bool readRatLsp(std::istream& in, std::pair<long long, unsigned long long>& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char open1, colon1, n_tag, colon2, d_tag, colon3, close, colon4;
        long long num;
        unsigned long long den;

        in >> open1 >> colon1 >> n_tag >> num >> colon2 >> d_tag >> den >> colon3 >> close >> colon4;

        if (open1 != '(' || colon1 != ':' || n_tag != 'N' ||
            colon2 != ':' || d_tag != 'D' || colon3 != ':' ||
            close != ')' || colon4 != ':') {
            in.setstate(std::ios::failbit);
            return false;
        }

        val = { num, den };
        return true;
    }

    int detectFieldType(std::istream& in) {
        std::streampos pos = in.tellg();
        char ch;
        in >> ch;
        in.seekg(pos);

        if (ch == '\'') return 4;
        if (ch == '#') return 5;
        if (ch == '(') return 6;
        if (ch == '"') return 7;
        if (ch == '0') {
            in >> ch;
            in.seekg(pos);
            in >> ch >> ch;
            if (ch == 'x' || ch == 'X') return 3;
            if (ch == 'b' || ch == 'B') return 3;
            if (ch >= '0' && ch <= '7') return 3;
            in.seekg(pos);
        }

        std::string temp;
        in >> temp;
        in.seekg(pos);

        if (temp.size() >= 2) {
            std::string suffix = temp.substr(temp.size() - 2);
            if (suffix == "ll" || suffix == "LL") return 2;
            if (suffix == "ul" || suffix == "UL") return 3;
        }
        if (temp.size() >= 1 && (temp.back() == 'd' || temp.back() == 'D')) return 1;
        if (temp.find_first_of("eE") != std::string::npos) return 1;

        return 0;
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;

        DataStruct input;

        if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' })) {
            return in;
        }

        bool has[3] = { false, false, false };
        int types[3] = { 0, 0, 0 };

        for (int i = 0; i < 3 && in; i++) {
            int keyNum = 0;
            in >> KeyIO{ keyNum };
            if (!in) break;

            in >> DelimiterIO{ ':' };
            if (!in) break;

            int fieldType = detectFieldType(in);

            if (keyNum == 1 && !has[0]) {
                if (fieldType == 1) {
                    if (readDblLit(in, input.key1_dbl) || readDblSci(in, input.key1_dbl)) {
                        input.key1_type = 1; has[0] = true; types[0] = 1;
                    }
                }
                else if (fieldType == 2) {
                    if (readSllLit(in, input.key1_sll)) {
                        input.key1_type = 2; has[0] = true; types[0] = 2;
                    }
                }
                else if (fieldType == 3) {
                    if (readUllLit(in, input.key1_ull) || readUllOct(in, input.key1_ull) ||
                        readUllBin(in, input.key1_ull) || readUllHex(in, input.key1_ull)) {
                        input.key1_type = 3; has[0] = true; types[0] = 3;
                    }
                }
                else if (fieldType == 4) {
                    if (readChrLit(in, input.key1_chr)) {
                        input.key1_type = 4; has[0] = true; types[0] = 4;
                    }
                }
                else if (fieldType == 5) {
                    if (readCmpLsp(in, input.key1_cmp)) {
                        input.key1_type = 5; has[0] = true; types[0] = 5;
                    }
                }
            }
            else if (keyNum == 2 && !has[1]) {
                if (fieldType == 1) {
                    if (readDblLit(in, input.key2_dbl) || readDblSci(in, input.key2_dbl)) {
                        input.key2_type = 1; has[1] = true; types[1] = 1;
                    }
                }
                else if (fieldType == 2) {
                    if (readSllLit(in, input.key2_sll)) {
                        input.key2_type = 2; has[1] = true; types[1] = 2;
                    }
                }
                else if (fieldType == 3) {
                    if (readUllLit(in, input.key2_ull) || readUllOct(in, input.key2_ull) ||
                        readUllBin(in, input.key2_ull) || readUllHex(in, input.key2_ull)) {
                        input.key2_type = 3; has[1] = true; types[1] = 3;
                    }
                }
                else if (fieldType == 4) {
                    if (readChrLit(in, input.key2_chr)) {
                        input.key2_type = 4; has[1] = true; types[1] = 4;
                    }
                }
                else if (fieldType == 5) {
                    if (readCmpLsp(in, input.key2_cmp)) {
                        input.key2_type = 5; has[1] = true; types[1] = 5;
                    }
                }
                else if (fieldType == 6) {
                    if (readRatLsp(in, input.key2_rat)) {
                        input.key2_type = 6; has[1] = true; types[1] = 6;
                    }
                }
            }
            else if (keyNum == 3 && !has[2]) {
                char quote;
                in >> quote;
                if (quote == '"') {
                    std::getline(in, input.key3, '"');
                    in >> DelimiterIO{ ':' };
                    has[2] = true;
                }
                else {
                    in.setstate(std::ios::failbit);
                }
            }
            else {
                in.setstate(std::ios::failbit);
                break;
            }
        }

        if (in) {
            in >> DelimiterIO{ ')' };
        }

        if (in && has[0] && has[1] && has[2]) {
            if (DataStruct::detected_variant == 0) {
                DataStruct::detected_variant = types[0] * 10 + types[1];
            }
            int currentVariant = types[0] * 10 + types[1];
            if (DataStruct::detected_variant == currentVariant || DataStruct::detected_variant == 0) {
                dest = input;
            }
            else {
                in.setstate(std::ios::failbit);
            }
        }
        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
        std::ostream::sentry sentry(out);
        if (!sentry) return out;

        out << "(:key1 ";

        switch (src.key1_type) {
        case 1: out << std::fixed << std::setprecision(1) << src.key1_dbl << "d"; break;
        case 2: out << src.key1_sll << "ll"; break;
        case 3: out << "0x" << std::hex << std::uppercase << src.key1_ull << std::dec; break;
        case 4: out << "'" << src.key1_chr << "'"; break;
        case 5: out << "#c(" << src.key1_cmp.real() << " " << src.key1_cmp.imag() << ")"; break;
        }

        out << ":key2 ";

        switch (src.key2_type) {
        case 1: out << std::fixed << std::setprecision(1) << src.key2_dbl << "d"; break;
        case 2: out << src.key2_sll << "ll"; break;
        case 3: out << src.key2_ull << "ull"; break;
        case 4: out << "'" << src.key2_chr << "'"; break;
        case 5: out << "#c(" << src.key2_cmp.real() << " " << src.key2_cmp.imag() << ")"; break;
        case 6: out << "(:N " << src.key2_rat.first << ":D " << src.key2_rat.second << ":)"; break;
        }

        out << ":key3 \"" << src.key3 << "\":)";
        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        double val1_a = 0, val1_b = 0;
        if (a.key1_type == 1) val1_a = a.key1_dbl;
        else if (a.key1_type == 2) val1_a = static_cast<double>(a.key1_sll);
        else if (a.key1_type == 3) val1_a = static_cast<double>(a.key1_ull);
        else if (a.key1_type == 4) val1_a = static_cast<double>(a.key1_chr);
        else if (a.key1_type == 5) val1_a = std::abs(a.key1_cmp);

        if (b.key1_type == 1) val1_b = b.key1_dbl;
        else if (b.key1_type == 2) val1_b = static_cast<double>(b.key1_sll);
        else if (b.key1_type == 3) val1_b = static_cast<double>(b.key1_ull);
        else if (b.key1_type == 4) val1_b = static_cast<double>(b.key1_chr);
        else if (b.key1_type == 5) val1_b = std::abs(b.key1_cmp);

        if (std::abs(val1_a - val1_b) > EPS) return val1_a < val1_b;

        double val2_a = 0, val2_b = 0;
        if (a.key2_type == 1) val2_a = a.key2_dbl;
        else if (a.key2_type == 2) val2_a = static_cast<double>(a.key2_sll);
        else if (a.key2_type == 3) val2_a = static_cast<double>(a.key2_ull);
        else if (a.key2_type == 4) val2_a = static_cast<double>(a.key2_chr);
        else if (a.key2_type == 5) val2_a = std::abs(a.key2_cmp);
        else if (a.key2_type == 6) val2_a = static_cast<double>(a.key2_rat.first) / a.key2_rat.second;

        if (b.key2_type == 1) val2_b = b.key2_dbl;
        else if (b.key2_type == 2) val2_b = static_cast<double>(b.key2_sll);
        else if (b.key2_type == 3) val2_b = static_cast<double>(b.key2_ull);
        else if (b.key2_type == 4) val2_b = static_cast<double>(b.key2_chr);
        else if (b.key2_type == 5) val2_b = std::abs(b.key2_cmp);
        else if (b.key2_type == 6) val2_b = static_cast<double>(b.key2_rat.first) / b.key2_rat.second;

        if (std::abs(val2_a - val2_b) > EPS) return val2_a < val2_b;

        return a.key3.length() < b.key3.length();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;

    while (std::cin) {
        std::copy(
            std::istream_iterator<nspace::DataStruct>(std::cin),
            std::istream_iterator<nspace::DataStruct>(),
            std::back_inserter(data)
        );

        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (std::cin.eof()) {
            break;
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
