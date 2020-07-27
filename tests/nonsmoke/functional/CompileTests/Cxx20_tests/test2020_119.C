// DQ (7/24/2020): the compare header file is required to support the <=> operator.
#include <compare>

int main() 
   {
     double foo = -0.0;
     double bar = 0.0;

     auto res = foo <=> bar; 
   }
