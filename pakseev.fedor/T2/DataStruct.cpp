#include "DataStruct.hpp"

#include <cmath>


const double EPSILON = 1.0e-9;

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
      return in;
    }

    char c = '0';
    in >> c;
    if (in && (c != dest.exp)) {
      in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, CharIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
      return in;
    }

    return in >> DelimiterIO{ '\'' } >> dest.ref >> DelimiterIO{ '\'' };
}

std::istream& operator>>(std::istream& in, DoubleIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    std::string mantissaStr;
    char exponentSign = '+';
    int exponent = 0;
    char symbol;
    bool hasDecimalPoint = false;
    bool validMantissa = false;
    bool hasDigitAfterPoint = false;
    char sign = '+';

    // Fix: added support for negative numbers
    if (in.peek() == '-') {
        in.get(sign);
    }

    while (in.get(symbol)) {
        if (isdigit(symbol)) {
            mantissaStr += symbol;
            validMantissa = true;
            if (hasDecimalPoint) {
                hasDigitAfterPoint = true;
            }
        } else if (symbol == '.' && !hasDecimalPoint) {
            if (mantissaStr.empty()) {
                in.setstate(std::ios::failbit);
                return in;
            }

            mantissaStr += symbol;
            hasDecimalPoint = true;

        } else if (symbol == 'e' || symbol == 'E') {
            break;
        } else {
            in.putback(symbol);
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if (!validMantissa || !hasDecimalPoint || !hasDigitAfterPoint) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (!in.get(exponentSign) || (exponentSign != '+' && exponentSign != '-')) {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> exponent;
    if (!in) {
        in.setstate(std::ios::failbit);
        return in;
    }

    double mantissa = std::stod(mantissaStr);
    int finalExponent = 0;
    if (exponentSign == '-') {
        finalExponent = -exponent;
    }
    else {
        finalExponent = exponent;
    }

    if (sign == '-') {
        mantissa = -mantissa;
    }

    dest.ref = mantissa * std::pow(10.0, finalExponent);
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    DataStruct temp;
    bool f1 = false, f2 = false, f3 = false;

    if (!(in >> DelimiterIO{ '(' })) {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (size_t i = 0; i < 3; ++i) {

        //Changed logic of parser: now we go until ":"
        if (!(in >> DelimiterIO{ ':' })) break;

        std::string label;
        if (!(in >> label)) break;

        if (label == "key1") {
            if (!(in >> DoubleIO{ temp.key1 })) break;
            f1 = true;
        }
        else if (label == "key2") {
            if (!(in >> CharIO{ temp.key2 })) break;
            f2 = true;
        }
        else if (label == "key3") {
            if (!(in >> StringIO{ temp.key3 })) break;
            f3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }
    }

    if (!(in >> DelimiterIO{ ':' } >> DelimiterIO{ ')' }) || !f1 || !f2 || !f3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = std::move(temp);
    return in;
}

bool comparator(const DataStruct& a, const DataStruct& b) {
    if (std::abs(a.key1 - b.key1) > EPSILON) { // Changed comparator, added EPSILON for double comp
        return a.key1 < b.key1;
    }

    else if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    else {
        return a.key3.length() < b.key3.length();
    }

}

std::ostream& operator<<(std::ostream& out, const DataStruct& data) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
      return out;
    }
    iofmtguard fmtguard(out);

    // Fix: added support for negative numbers
    double mantissa = data.key1;
    int exponent = 0;

    if (mantissa == 0.0) { // Fix: if mantissa 0.0 output is now 0.0e+0 not 1.0e+0
        exponent = 0;
    } else {
        bool negative = mantissa < 0;
        mantissa = std::abs(mantissa);

        while (mantissa < 1.0) {
            mantissa *= 10.0;
            exponent--;
        }

        while (mantissa >= 10.0) {
            mantissa /= 10.0;
            exponent++;
        }

        if (negative) {
            mantissa = -mantissa;
        }
    }

    out << "(:";
    out << "key1 ";
    out << std::fixed << std::setprecision(1) << mantissa;
    if (exponent >= 0) {
        out << "e+" << exponent;
    } else {
        out << "e" << exponent;
    }
    out << ":key2 \'" << data.key2 << "\'";
    out << ":key3 \"" << data.key3 << "\"";
    out << ":)";
    return out;
}

iofmtguard::iofmtguard(std::basic_ios< char >& s) :
    s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
{}
iofmtguard::~iofmtguard() {
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}
