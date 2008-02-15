// Hi Dan,
// Rose built without a problem, thanks for sending me the distribution.
// Here is a program I am having trouble with :

// ------------ begin rctest.cc --------------------

#include <iostream>

// #include "A++.h" // this caused the original problem

struct intArray
   {
     intArray(const int&);
   };

bool operator||(int, const intArray &);
bool operator||(const intArray &,int);

using namespace std;
int main(int argc, char *argv[])
   {
     int i = 10;

  // this is the line where the complaint gets generated:
     cout << i << endl;

     return 0;
   }

// ------------ end rctest.cc --------------------

/*
I tried to compile this code with the identity translator that
comes with the version of rose you gave me.  I got the following
output (and a core file):

and}254: ../rose_linux_gcc3.3.2/bin/identityTranslator -c rctest.cc sourceFile = rctest.cc
Found a ".cc" source file!
"/home/chand/scratch/rose/rose_linux_gcc3.3.2/g++_HEADERS/hdrs1/ostream", line 206: error:
          more than one operator "||" matches these operands:
            function "operator||(int, const intArray &)"
            function "operator||(const intArray &, int)"
            operand types are: std::_Ios_Fmtflags || std::_Ios_Fmtflags
        if (__fmt & ios_base::oct || __fmt & ios_base::hex)
                                  ^
          detected during instantiation of "std::basic_ostream<_CharT,
                    _Traits>::__ostream_type &std::basic_ostream<_CharT,
                    _Traits>::operator<<(int) [with _CharT=char,
                    _Traits=std::char_traits<char>]" at line 19 of "rctest.cc"
 
Errors in EDG Processing!
Abort (core dumped)

gcc 3.3.2 compiles this code.

*/
