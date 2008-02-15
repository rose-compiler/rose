// This test code demonstrates a bug in ROSE where the throw expression 
// takes a type which does not implement an explicit constructor.

class ErrorWithExplicitConstructor
   {
  // The explicit presence of a constructor allows for the constructor to be used in the throw (below)
  // but with out it the throw expression is a current bug in ROSE.  
     public:
          ErrorWithExplicitConstructor();
   };

class ErrorWithOutExplicitConstructor {};

class B
   {
     public:
          void foo1() throw(ErrorWithExplicitConstructor)
             {
               if (true)
                  {
                 // Throw expression taking a constructor for a class which 
                 // does not implement an explicit default constructor.
                    throw ErrorWithExplicitConstructor();
                  }
             }

          void foo2() throw(ErrorWithOutExplicitConstructor)
             {
               if (true)
                  {
                 // Throw expression taking a constructor for a class which 
                 // does not implement an explicit default constructor.
                    throw ErrorWithOutExplicitConstructor();
                  }
             }
   };

#if 1
// Pointer to a function with a throw specification (not supported in ROSE)
// void (*pf)(int) throw(std::string, int);
void (*pf_0)(int) throw();
void (*pf_1)(int) throw(int);
void (*pf_2)(int) throw(int,long);
void (*pf_3)(int) throw(int,long,bool);
#endif
