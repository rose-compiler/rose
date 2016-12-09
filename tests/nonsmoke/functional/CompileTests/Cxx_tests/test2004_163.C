#include<stdio.h>

#if 0
// Common example from web (also from http://developer.apple.com/documentation/DeveloperTools/gcc-3.3/gcc/Variable-Length.html)
FILE *concat_fopen (char *s1, char *s2, char *mode)
{
  char str[strlen (s1) + strlen (s2) + 1];
  strcpy (str, s1);
  strcat (str, s2);
  return fopen (str, mode);
}

// This is a C++ example from the web, Questionable support for this in g++
class foo
{
  static const int i = 10;
  int array[i];
};


// From: http://developer.apple.com/documentation/DeveloperTools/gcc-3.3/gcc/Variable-Length.html
struct entry
     tester (int len, char data[len][len])
     {
       /* ... */
     }
// The length of an array is computed once when the storage is allocated and is remembered 
// for the scope of the array in case you access it with sizeof.

// If you want to pass the array first and the length afterward, you can use a forward 
// declaration in the parameter list--another GNU extension.

     struct entry
     tester (int len; char data[len][len], int len)
     {
       /* ... */
     }
     

// The int len before the semicolon is a parameter forward declaration, and it serves the 
// purpose of making the name len known when the declaration of data is parsed.

// You can write any number of such parameter forward declarations in the parameter list. 
// They can be separated by commas or semicolons, but the last one must end with a semicolon, 
// which is followed by the "real" parameter declarations. Each forward declaration must match 
// a "real" declaration in parameter name and data type. ISO C99 does not support parameter 
// forward declarations. 

#endif

