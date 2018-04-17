// This test code demonstrates a bug in ROSE where the throw expression 
// takes a type which does not implement an explicit constructor.

class ErrorWithOutExplicitConstructor {};

class B
   {
     public:
          void foo2() throw(ErrorWithOutExplicitConstructor)
             {
               if (true)
                  {
                 // Throw expression taking a constructor for a class which 
                 // does not implement an explicit default constructor.
#if 1
                    throw ErrorWithOutExplicitConstructor();
#endif
                  }
             }
   };

