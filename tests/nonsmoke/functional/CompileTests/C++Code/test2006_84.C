// This example code demonstrates the use of throw specifiers
// for function pointers and there use in restrictive assignments.

#include <string>

using namespace std;

// Not sure what it takes to make this work. Where are "bad_code" and "no_auth" found?
// void validate(int code) throw (bad_code, no_auth);

void (*pf)(int) throw(string, int);

// more restrictive than pf:
void e(int) throw (string); 
// as restrictive as pf:
void f(int) throw (string, int); 
// less restrictive than pf:
void g(int) throw (string, int, bool); 

void foo()
   {
     pf=e; //fine

     pf=f; //fine

  // EDG correctly reports this as an error!
  // pf=g; //error
   }
