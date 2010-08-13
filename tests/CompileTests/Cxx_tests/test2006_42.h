#include <iostream>

using namespace std;

class Bar {
public:
   Bar();
   // Check that we cover function definition given in the class declaration.
   void bar() {cout<<"Bar.bar()"<<endl;} /***Cov bb*/ /***Cov me*/
   int moo();
};
