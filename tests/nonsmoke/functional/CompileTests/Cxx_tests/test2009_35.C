/*
   This code is problematic because of the defining member function declaration whose body 
includes a reference to std::cout.  If one adds a global function that precedes the declaration 
of class A and whose body contains a reference to std::cout, ROSE can successfully parse the 
program.  Alternatively, if one changes foo to be a non-defining declaration, and then adds 
a definition outside of the declaration of the class, the problem again goes away.

 */

#include <iostream>
class A
   {
     void foo() { std::cout << "hejsan" << std::endl; }
   };

int main()
   {
     return 0;
   }
