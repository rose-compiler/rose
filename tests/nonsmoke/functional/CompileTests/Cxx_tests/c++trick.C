/* 
Hi Dan,
   I got this C++ code, and it reminded me of the question you asked me 
when I interviewed with LLNL last year, about the template, specialized 
template and non-template functions selection. I thought you might get 
a kick out of it. I attached the code.
 */

#include <iostream>

template <class T>
void print(const T& a, const T& b) 
   {
     std::cout << "Templated function: " << a << ", " << b << std::endl;
   }

void print(char* a, char* b)
   {
     std::cout << "Regular function: " << a << ", " << b << std::endl;
   }

int main()
   {
     print("Hello", "World");
     print("Hello", "Word");
   }
