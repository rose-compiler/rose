
// int abs(int x) { if (x < 0) return -x; else return x; }

int abs(int x) { if (x < 0) return x; else return x; }

class A
   {
     int x;
     A(): x(42) {}      
   };

int main()
   {
  // We unparse too many parenthesis: "int x = (abs(0));"
     int x = abs(0);

  // Note that return statement from main appears to be optional!
  // return 0;
   }
