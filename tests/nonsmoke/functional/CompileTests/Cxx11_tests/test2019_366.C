#include<string>

typedef long unsigned size_t;

long double operator "" _w(long double);
std::string operator "" _w(const char16_t*, size_t);
unsigned operator "" _w(const char*);
int main() 
   {
     1.2_w;    // calls operator "" _w(1.2L)
     u"one"_w; // calls operator "" _w(u"one", 3)
     12_w;     // calls operator "" _w("12")
  // "two"_w;  // error: no applicable literal operator
   }
