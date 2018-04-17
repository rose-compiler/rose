#include <iostream>
#include <string>
std::string str("hello");
enum MyEnumType { ALPHA, BETA, GAMMA };

union AnyType {
    int a;
      double Number;
};
int main()
{
  for (int i= 0; i< str.size(); i++)
  {
    union AnyType a;

    if (i==ALPHA)
    std::cout<<str[i]<<std::endl;
  }
  return 0; 
}
