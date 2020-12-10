class A
   {
     public:
       // inline void ba_foo (bool value)
          void ba_foo()
             {
            // ab_foo = value; 
               ab_foo; 
             }

  // protected:
     private:
       // The outlining of this protected member will force the outlined function to be declared as a friend function.
       // The compilation of the *_lib file will require use of the unparse headers option?
          bool ab_foo;
   };

