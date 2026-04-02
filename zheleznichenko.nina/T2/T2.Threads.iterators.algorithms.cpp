#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>
#include <iterator>
#include <iomanip>

struct DataStruct {
  unsigned long long key1;
  std::complex<double> key2;
  std::string key3;
};

void skipChar(std::istream& in, char exp) {
  char c;
  in >> c;
  if (c != exp) in.setstate(std::ios::failbit);
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
  std::string line;
  if (!(in >> std::ws) || in.peek() != '(') return in;

  skipChar(in, '(');
  skipChar(in, ':');

  DataStruct temp;
  for (int i = 0; i < 3; ++i) {
    std::string label;
    std::getline(in, label, ' ');

    if (label == "key1") {
      skipChar(in, '0'); skipChar(in, 'b');
      std::string bin;
      while (std::isdigit(in.peek())) {
        char c; in >> c; bin += c;
      }
      temp.key1 = std::stoull(bin, nullptr, 2);
    }
    else if (label == "key2") {
      double re, im;
      skipChar(in, '#'); skipChar(in, 'c'); skipChar(in, '(');
      in >> re >> im;
      skipChar(in, ')');
      temp.key2 = { re, im };
    }
    else if (label == "key3") {
      skipChar(in, '"');
      std::getline(in, temp.key3, '"');
    }
    skipChar(in, ':');
  }
  skipChar(in, ')');

  if (in) dest = temp;
  return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
  out << "(:key1 0b";
  std::string b;
  unsigned long long n = src.key1;
  if (n == 0) b = "0";
  while (n > 0) { b += (n % 2 ? '1' : '0'); n /= 2; }
  std::reverse(b.begin(), b.end());

  out << b << ":key2 #c(" << std::fixed << std::setprecision(1)
    << src.key2.real() << " " << src.key2.imag() << "):key3 \""
    << src.key3 << "\":)";
  return out;
}

int main() {
  std::vector<DataStruct> data;

  std::copy(std::istream_iterator<DataStruct>(std::cin),
    std::istream_iterator<DataStruct>(),
    std::back_inserter(data));

  std::sort(data.begin(), data.end(), [](const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (std::abs(a.key2) != std::abs(b.key2)) return std::abs(a.key2) < std::abs(b.key2);
    return a.key3.length() < b.key3.length();
    });

  std::copy(data.begin(), data.end(),
    std::ostream_iterator<DataStruct>(std::cout, "\n"));

  return 0;
}
