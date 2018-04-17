// g0016.cc
// gcc allows some nonstandard variations among declarations
// if the first declarations is in a preprocessing section
// where the #line header ends with "3" (exactly)

// This input file exists as a record of what I have determined
// gcc's behavior to be.  Elsa handles the problem in a different
// way than by emulating gcc.

// this would provoke an error in gcc-3, but gcc-2 accepts it, so I
// am not going to test the behavior of Elsa on it
//# 1 "butnot.h"
//int isatty  ( int )  ;

# 5 "somewhere.h" 3

// an extra intervening #line apparently doesn't change prevailing
// filename nor flags
# 19

// this declaration has "throw()" whereas the rest do not; according
// to cppstd, that is an error
int isatty (int __fd) throw () ;

# 7 "else.h"

int isatty  ( int )  ;

# 9 "again.h"

int isatty  ( int )  ;
