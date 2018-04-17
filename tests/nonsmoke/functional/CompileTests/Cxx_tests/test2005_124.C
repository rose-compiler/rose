
class A
   {
     public:
          A();
          A(int x);
          A(float x, float y);
   };

class B {};

#if 0
// This is correctly unparsed as "class A objectA1();"
A objectA1;

// This appears as a function declaration instead of a variable declaration, unparsed as "extern class A objectA2();"
A objectA2_function();

class A objectA2_object();

// This is correctly unparsed as "class B objectB2;"
B objectB2 = B();

// This is unparsed as "class A objectA3;"  This is OK, since front-ends are permitted such optimizations.
A objectA3 = A();

// This is unparsed as "class A objectA4(1);"  This is OK, since front-ends are permitted such optimizations.
A objectA4 = A(1);

// This is unparsed as "class A objectA5 = (objectA1);"
A objectA5 = objectA1;

char* copyString = new char[1];

#endif

A objectA6 = A(1.0,6.0);
A objectA7 = A(1);

A objectA8(1.0,6.0);

// These are not paired 
#if 0
#include<list>
#include<vector>
#include<map>

using namespace std;

list<int> integerList;
list<int>::iterator i = integerList.begin();
// These are not paired 
#endif
