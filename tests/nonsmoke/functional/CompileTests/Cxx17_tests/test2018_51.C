// Support for hexadecimal floating point numbers.

#include <iostream>
int main()
{
  std::cout << 58.         << '\n'
            << 4e2         << '\n'
            << 123.456e-67 << '\n'
            << .1E4f       << '\n'
            << 0x10.1p0    << '\n';
}

