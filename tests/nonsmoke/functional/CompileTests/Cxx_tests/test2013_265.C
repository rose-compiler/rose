// This is a bug that was reported by Christian, but it does not appear to be
// a problem for ROSE currently..

// #include <iostream>

class Arithmetics
   {
     public:
          int add(int a, int b);
   };

int Arithmetics::add(int a, int b)
   {
     return a + b;
   }

int main(int argc, char** argv)
   {
        int a = 3;
        int b = 4;
        int result = 0;
        Arithmetics ariths;
        result = ariths.add(a, b);
//      std::cout << result << std::endl;

        return 0;
   }
