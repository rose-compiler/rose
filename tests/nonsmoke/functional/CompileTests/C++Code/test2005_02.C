// Test for calls to templated member functions
#include <stdio.h>

// test template function
template <typename T>
void foo3(void) {}

// This is the first of two overloaded functions which are identical
// but it is called by:
//      foo4(1.0);
//      foo4(1);
//      foo4<int>(1);
template <typename T>
void foo4( T x ) // mangled name: foo4__Fd_v
   {
     printf ("1st overloaded function! \n");
   }

// This is the second of two overloaded functions which are identical
// but it is called by:
//      foo4<double>(1.0);
template <typename T>
void foo4( double x ) // mangled name: foo4__Fi_v
   {
     printf ("2nd overloaded function! \n");
   }

// Note that by putting out these prototypes we don't generate the template function definitions!
// But it should force the symbol to be undefined at link time which will force the instantiation
// (as I recall).
// template <> void foo3< char > ();
// template <> void foo4< double > (double x);
// template <> void foo4< int > (int x);
// template <> void foo4< double > (double x);

int main()
   {
     foo3<char>();
  // foo3();  // This is an error, since it does not contain enough information for type resolution

     foo4(1.0);
     foo4(1);
     foo4<double>(1.0);
     foo4<int>(1);

     return 0;
   }


// Output:
// 1st overloaded function! 
// 1st overloaded function! 
// 2nd overloaded function! 
// 1st overloaded function! 



/*
Possible explaination (sent by Markus and found at:
     http://static.cray-cyber.org/Documentation/NEC_SX_R10_1/G1AF03E/NODE14.3.HTML

A template function may be overloaded either by (other) functions of its name 
or by (other) template functions of that same name. Overloading resolution 
for template functions and other functions of the same name is done in three 
steps:

    * [1] Look for an exact match (13.2) on functions; if found, call it.
    * [2] Look for a function template from which a function that can be 
called with an exact match can be generated; if found, call it.
    * [3] Try ordinary overloading resolution (13.2) for the functions; if a 
function is found, call it. 

If no match is found the call is an error. In each case, if there is more than 
one alternative in the first step that finds a match, the call is ambiguous 
and is an error.

*/
