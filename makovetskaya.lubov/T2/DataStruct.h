#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

struct DataStruct {
    double key1;
    signed long long key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};

struct Double {
    double& ref;
};

struct SLongLongIO {
    signed long long& ref;
};

struct StringIO {
    std::string& ref;
};

class iofmtguard {
public:
    explicit iofmtguard(std::basic_ios<char>& s);
    ~iofmtguard();

private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::ios::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, Double&& dest);
std::istream& operator>>(std::istream& in, SLongLongIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& src);

#endif
