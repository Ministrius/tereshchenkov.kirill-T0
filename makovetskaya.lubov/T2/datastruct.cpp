#include "DataStruct.h"

std::istream& operator>>(std::istream& in, DelimiterIO&& dest){
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

std::istream& operator>>(std::istream& in, Double&& key){
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    in >> key.ref;
    if (in) {
        char c = in.peek();
        if (c == 'd' || c == 'D') {
            in.get();
        } else {
            in.setstate(std::ios::failbit);
        }
    }
    return in;
}

std::istream& operator>>(std::istream& in, SLongLongIO&& key){
    std::istream::sentry sentry(in);
    if (!sentry){
        return in;
    }
    in >> key.ref;
    if (in) {
        char next = in.peek();
        if (next == 'L' || next == 'l'){
            in.get();
            char next2 = in.peek();
            if (next2 == 'L' || next2 == 'l'){
                in.get();
            }
            else{
                in.setstate(std::ios::failbit);
            }
        }
        else {
            in.setstate(std::ios::failbit);
        }
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest){
    std::istream::sentry sentry(in);
    if (!sentry){
        return in;
    }
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, DataStruct& data){
    std::istream::sentry sentry(in);
    if (!sentry){
        return in;
    }
    size_t currKey = 0;
    DataStruct tmp = { 0, 0, "" };
    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;
    const size_t MAX_KEYS = 3;
    in >> DelimiterIO{ '(' };
    for (size_t i = 0; i < MAX_KEYS && in; ++i){
        in >> DelimiterIO{ ':' } >> DelimiterIO{ 'k' } >> DelimiterIO{ 'e' } >> DelimiterIO{ 'y' };
        in >> currKey;
        if (currKey == 1 && !hasKey1){
            in >> Double{ tmp.key1 };
            hasKey1 = true;
        }
        else if (currKey == 2 && !hasKey2){
            in >> SLongLongIO{ tmp.key2 };
            hasKey2 = true;
        }
        else if (currKey == 3 && !hasKey3){
            in >> StringIO{ tmp.key3 };
            hasKey3 = true;
        }
        else{
            in.setstate(std::ios::failbit);
        }
    }
    in >> DelimiterIO{ ':' } >> DelimiterIO{ ')' };
    if (in && hasKey1 && hasKey2 && hasKey3){
        data = tmp;
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src){
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }
    iofmtguard g(out);
    out << "(:key1 ";
    out << std::fixed << std::setprecision(1) << src.key1 << "d:";
    out << "key2 " << src.key2 << "ll:";
    out << "key3 \"" << src.key3 << "\":)";
    return out;
}

iofmtguard::iofmtguard(std::basic_ios<char>& s): s_(s), width_(s.width()), fill_(s.fill()),
precision_(s.precision()), fmt_(s.flags()){}

iofmtguard::~iofmtguard(){
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}
