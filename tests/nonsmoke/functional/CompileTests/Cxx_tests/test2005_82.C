// Testing the ability to find the use of a class in the same file!
// Part of a test required to know when to output template and nested classes of templates.
/*
template <typename T>
class X
   {
     T a;
   };

X<int> x;
*/

#include<list>

int main()
   {
     std::list<int> integerList;
     return 0;
   }


