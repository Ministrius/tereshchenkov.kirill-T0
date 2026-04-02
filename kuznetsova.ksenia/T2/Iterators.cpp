#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <limits>
#include <complex>

namespace nspace {
const double EPS = 1e-9;
struct DataStruct {
unsigned long long key1 = 0;
std::complex<double> key2{ 0.0, 0.0 };
std::string key3;
};

struct DelimiterIO {
char exp;
};

struct KeyIO {
int& ref;
};

struct UllHexIO {
unsigned long long& ref;
};

struct CmpLspIO {
std::complex<double>& ref;
};

std::istream& operator>>(std::istream& in, KeyIO&& dest) {
std::istream::sentry sentry(in);
if (!sentry) return in;
char k, e, y, n;
in >> k >> e >> y >> n;
if (in && (k != 'k' || e != 'e' || y != 'y' || !(n >= '1' && n <= '3'))) {
in.setstate(std::ios::failbit);
}
else {
dest.ref = n - '0';
}

return in;
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
std::istream::sentry sentry(in);
if (!sentry) return in;
char c;
in >> c;
if (in && std::tolower(c) != std::tolower(dest.exp)) {
in.setstate(std::ios::failbit);
}
return in;
}

std::istream& operator>>(std::istream& in, UllHexIO&& dest) {
std::istream::sentry sentry(in);
if (!sentry) return in;

char prefix1, prefix2;
in >> prefix1 >> prefix2;
if (prefix1 != '0' || (prefix2 != 'x' && prefix2 != 'X')) {
in.setstate(std::ios::failbit);
return in;
}

unsigned long long value;
if (!(in >> std::hex >> value)) {
in.setstate(std::ios::failbit);
return in;
}
in >> std::dec;

char colon;
in >> colon;
if (colon != ':') {
in.setstate(std::ios::failbit);
}
else {
dest.ref = value;
}
return in;
}

std::istream& operator>>(std::istream& in, CmpLspIO&& dest) {
std::istream::sentry sentry(in);
if (!sentry) return in;

char hash, c, open, close, colon;
double real, imag;

in >> hash >> c >> open;
if (hash != '#' || c != 'c' || open != '(') {
in.setstate(std::ios::failbit);
return in;
}

if (!(in >> real >> imag)) {
in.setstate(std::ios::failbit);
return in;
}

in >> close >> colon;
if (close != ')' || colon != ':') {
in.setstate(std::ios::failbit);
return in;
}

dest.ref = std::complex<double>(real, imag);
return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
std::istream::sentry sentry(in);
if (!sentry) return in;

DataStruct input;

if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' })) {
return in;
}

bool has[3] = { false, false, false };

for (int i = 0; i < 3 && in; i++) {
int keyNum = 0;
in >> KeyIO{ keyNum };
if (!in) break;

in >> DelimiterIO{ ':' };
if (!in) break;

if (keyNum == 1 && !has[0]) {
in >> UllHexIO{ input.key1 };
has[0] = true;
}
else if (keyNum == 2 && !has[1]) {
in >> CmpLspIO{ input.key2 };
has[1] = true;
}
else if (keyNum == 3 && !has[2]) {
char quote;
in >> quote;
if (quote != '"') {
in.setstate(std::ios::failbit);
break;
}
std::getline(in, input.key3, '"');
in >> DelimiterIO{ ':' };
has[2] = true;
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
dest = input;
}
return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
std::ostream::sentry sentry(out);
if (!sentry) return out;

out << "(:key1 0x" << std::hex << std::uppercase << src.key1 << std::dec << std::nouppercase;

out << ":key2 #c(" << std::fixed << std::setprecision(1)
<< src.key2.real() << " " << src.key2.imag() << ")";

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
return 1;
}

std::sort(data.begin(), data.end(), nspace::compareData);

std::copy(
data.begin(),
data.end(),
std::ostream_iterator<nspace::DataStruct>(std::cout, "\n")
);
}
