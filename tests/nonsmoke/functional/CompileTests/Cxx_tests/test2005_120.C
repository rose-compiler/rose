// MIlind Chabbi (9/17/2013): Changed to sane names to avoid conflict with global name space.
// int rose_test_abs(int x) { if (x < 0) return -x; else return x; }

int rose_test_abs(int x) { if (x < 0) return x; else return x; }

class A
   {
     int x;
     A(): x(42) {}      
   };

int main()
   {
  // We unparse too many parenthesis: "int x = (rose_test_abs(0));"
     int x = rose_test_abs(0);

  // Note that return statement from main appears to be optional!
  // return 0;
   }
