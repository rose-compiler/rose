
// This issue corresponds to the assertion failing at line 3006 of the AstConsistancyTests.C file.
// This is tied to be the 3rd most numerous issue in testing (causing failure in 3 files).

extern const double ZERO;

template <const double & T, const double & S = ZERO>
class A
   {
     public: 
          A();
     private:
          int value;
   };

template <const double & T, const double & S>
A<T,S>::A()
   {
     value = 0;
   }

