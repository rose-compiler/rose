// This code demonstrates a bug in the EDG/SAGE connection's 
// handling of "Zero initialization", as in "A()" when A has no constructor

// Andreas thinks it is a problem when there is a defined constructor as well.

typedef struct var21
   {
     int x;
   } var1;

int main(int argc, char* argv[])
   {
  // var1* test = 0; // new var1();   WORKS
  // var1* test = new var1;           WORKS
      var1* test = new var1();     // FAILS   
     return 0;
   }
