// DQ (include file bar.h included directly below)
// #include "bar.h"

#include <iostream>

using namespace std;

class Bar {
public:
   Bar();
   // Check that we cover function definition given in the class declaration.
   void bar() { /* cout<<"Bar.bar()"<<endl; */} /***Cov bb*/ /***Cov me*/
   int moo();
};

//Check that we cover empty constructor block
Bar::Bar() {} /***Cov bb*/ /***Cov me*/

int Bar::moo() { /***Cov bb*/ /***Cov me*/
   try { /***Cov bb*/
//    cout<<"Bar::moo()"<<endl;
      return 1;
   } catch(...) { /***Cov bb ~ */
      return 2;
   }
}

extern "C"
void bar() { /***Cov bb*/ /***Cov me*/
   Bar b;
   b.bar();
   b.moo();
}
