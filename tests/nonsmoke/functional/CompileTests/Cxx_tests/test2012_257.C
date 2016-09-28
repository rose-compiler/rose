#include <new>

using namespace std;

// const nothrow_t std::nothrow;

void foobar() 
   {
     int length = 10;

  // int *i = new (nothrow) int[10]; // works

  // Unparses as:  int *i = new (length * 4UL,nothrow) int [length * 4UL];
     int *i = new (nothrow) int[length];
   }
