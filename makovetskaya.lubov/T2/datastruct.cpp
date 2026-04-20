#include "DataStruct.h"
#include <cmath>
#include <cctype>

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    char c = '0';
    in >> c;
    if (c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleIO&& key) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    in >> key.ref;
    if (!in) {
        return in;
    }

    char suffix = ' ';
    if (in >> suffix) {
        if (suffix != 'd' && suffix != 'D') {
            in.setstate(std::ios::failbit);
            return in;
        }
        char next = in.peek();
        if (next != ':' && next != ' ' && next != '\n' && next != EOF) {
            in.setstate(std::ios::failbit);
        }
    } else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, ULongLongIO&& key) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    in >> key.ref;
    if (!in) {
        return in;
    }

    char firstChar = ' ';
    char secondChar = ' ';
    char thirdChar = ' ';

    if (in >> firstChar) {
        if (firstChar == 'U' || firstChar == 'u') {
            if (in >> secondChar) {
                if (secondChar == 'L' || secondChar == 'l') {
                    if (in >> thirdChar) {
                        if (thirdChar == 'L' || thirdChar == 'l') {
                            char next = in.peek();
                            if (next != ':' && next != ' ' && next != '\n' && next != EOF) {
                                in.setstate(std::ios::failbit);
                            }
                        }
                        else {
                            in.setstate(std::ios::failbit);
                        }
                    }
                    else {
                        in.setstate(std::ios::failbit);
                    }
                }
                else {
                    in.setstate(std::ios::failbit);
                }
            }
            else {
                in.setstate(std::ios::failbit);
            }
        }
        else {
            in.setstate(std::ios::failbit);
        }
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}


std::istream& operator>>(std::istream& in, DataStruct& data) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    DataStruct tmp;
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;

    in >> DelimiterIO{ '(' };

    while (in && (!hasKey1 || !hasKey2 || !hasKey3)) {
        in >> DelimiterIO{ ':' };

        std::string label;
        in >> label;

        if (!in) {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (label == "key1" && !hasKey1) {
            in >> DoubleIO{ tmp.key1 };
            hasKey1 = true;
        }
        else if (label == "key2" && !hasKey2) {
            in >> ULongLongIO{ tmp.key2 };
            hasKey2 = true;
        }
        else if (label == "key3" && !hasKey3) {
            in >> StringIO{ tmp.key3 };
            hasKey3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }
    }

    in >> DelimiterIO{ ':' } >> DelimiterIO{ ')' };

    if (in && hasKey1 && hasKey2 && hasKey3) {
        data = tmp;
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }
    iofmtguard g(out);

    out << "(:key1 ";
    out << std::fixed << std::setprecision(1) << src.key1 << "d:";
    out << "key2 " << src.key2 << "ull:";
    out << "key3 \"" << src.key3 << "\":)";

    return out;
}

iofmtguard::iofmtguard(std::basic_ios<char>& s) :
    s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags()) {
}

iofmtguard::~iofmtguard() {
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}
