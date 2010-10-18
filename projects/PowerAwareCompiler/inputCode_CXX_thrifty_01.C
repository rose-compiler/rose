// Trivial Thrifty tests code for running on the simulator.

class test_cxx
   {
     public:
          int foo() 
            {
               return 0;
            }
   };

int main( int argc, char* arcgv[] )
   {
     test_cxx X;
     return X.foo();
   }
