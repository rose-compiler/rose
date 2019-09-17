
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <cassert>

std::string str_f(float x)
{
  std::string       res;
  std::stringstream buf;

  buf << std::setprecision (std::numeric_limits<float>::digits10 + 2) 
      << x;
  buf >> res;
  
  return res;
}

int main()
{
  std::string a = str_f(1.4-1);
  std::string b = str_f(1.4f-1.0f);

  assert(  sizeof(double) == sizeof(float)
        || a != b
        );
  return 0;
}
