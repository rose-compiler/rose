// This bug corresponds to the assertion failing in Insert.cc of ROSE (outlining support).
// This is the 1st most numerous issue in testing (causing failure in 87 files).

class A
   {
     public:
          inline void ba_foo (bool value)
             {
               ab_foo = value; 
             }

  // protected:
     private:
       // The outlining of this protected member will force the outlined function to be declared as a friend function.
       // The compilation of the *_lib file will require use of the unparse headers option?
          bool ab_foo;
   };

