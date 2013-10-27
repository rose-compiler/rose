
// Note: Must have space between the "include" and the "<filename>" substrings
// #include<stdlib.h>
// #include<stdio.h>
#include <assert.h>

// Unparser Bug (currently addressed by test2001_06.C)

// 2001/1/6 DQ Error in unparsing:
//   Original code:
//      ((Array_Domain_Type*)(this))->IndexBase  [i] = Index_Array[i]->Array_Descriptor.Array_Domain.Base   [i];
//   Unparsed code:
//      ((Array_Domain_Type & )(*this).IndexBase)[i] = ((((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Base)[i];

// DQ (10/26/2013): added to support debuging of aterm support.
extern void __assert (const char *__assertion, const char *__file, int __line) __THROW __attribute__ ((__noreturn__));

int main()
   {
     __PRETTY_FUNCTION__;
     __func__;
     assert (1 == 1);
     // printf ("Program Terminated Normally! \n");
     return 0;
   }

