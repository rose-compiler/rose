#include <typeinfo>

int main() 
   {
  // built-in types:
     int i;
     int * pi;
     typeid(int).name();
     typeid(i).name();
     typeid(pi).name();
     typeid(*pi).name();

     bool test = ( typeid(derived)==typeid(*pbase) );
   }

