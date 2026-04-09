#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <iterator>
#include <vector>
#include <iomanip>

struct DataStruct {
    double key1;
    char key2;
    std::string key3;
};

struct DelimiterIO {
  char exp;
};

struct DoubleIO {
  double& ref;
};

struct StringIO {
  std::string& ref;
};

struct CharIO {
  char& ref;
};


class iofmtguard {
public:
  iofmtguard(std::basic_ios<char>& s);
  ~iofmtguard();
private:
  std::basic_ios<char>& s_;
  std::streamsize width_;
  char fill_;
  std::streamsize precision_;
  std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DoubleIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, CharIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& os, const DataStruct& data);

bool comparator(const DataStruct& a, const DataStruct& b);

#endif
