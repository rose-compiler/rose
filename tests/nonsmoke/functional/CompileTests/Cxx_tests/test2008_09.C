// Test CPP directives.

// Example from Ftest2008_05.f90
# 123 "<built-in>"

#if 1

// C and C++ CPP can have leading whitespace before CPP directives (Fortran can't).
  #define X 4
  #else
  #warning "xxx"
#endif

// C and C++ CPP can have leading whitespace before CPP directives (Fortran can't).
      #   line 10 "foobar_header_file"

#   line 10 "foobar_header_file"
#define N 7

