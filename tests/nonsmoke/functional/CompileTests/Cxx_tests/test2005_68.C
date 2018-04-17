// With the new name mangling in place the use of variables 
// before they are defined is not working for classes.

#define DEMONSTRATE_BUG 1

int x;

class X
   {
     public:
          int y;

          static 
          bool _S_initialized()
             { 
#if DEMONSTRATE_BUG
            // A variable declaration will be created for the first occurance of 
            // _S_ios_base_init but it will not be connected to the AST!
               return ::X::_S_ios_base_init;
#else
               return true;
#endif
             }

          int z;

     private:
       // We need to have the previously used variable declaration be used here instead 
       // of a new one being created.  Either that or force the previous initialized name
       // (called prev_decl_item in Sage III)  to point to the correct declaration (this one).
          static int _S_ios_base_init;
       // int _S_ios_base_init;
   };

#if 1
// DQ (1/7/2007): added initalization of static variable so test name qualification.
int X::_S_ios_base_init = 0;
#endif

#if 1
// DQ (1/7/2007): added initalization of static variable so test name qualification.
namespace Y
   {
     class X
        {
          void beforeDeclaration() 
             {
               ::Y::X::_S_ios_base_init = 0;
               if (::Y::X::_S_ios_base_init == 0);
               while (::Y::X::_S_ios_base_init == 0);
             }

          void secondFunctionBeforeDeclaration() 
             {
               _S_ios_base_init = 0;
               if (_S_ios_base_init == 0);
               while (_S_ios_base_init == 0);
             }

          public: static int _S_ios_base_init;

          void afterDeclaration() 
             {
               ::Y::X::_S_ios_base_init = 0;
               if (::Y::X::_S_ios_base_init == 0);
               while (::Y::X::_S_ios_base_init == 0);
             }

          class X2
             {
               void beforeDeclaration() 
                  {
                    ::Y::X::_S_ios_base_init = 0;
                    if (::Y::X::_S_ios_base_init == 0);
                    while (::Y::X::_S_ios_base_init == 0);
                  }

               public: static int _S_ios_base_init;
             };
        };

  // Note that this can be defined in either global or namespace scope
  // int X::_S_ios_base_init = 0;
  // int _S_ios_base_init = 0;
   }
#if 1
// Note that this can be defined in either global or namespace scope
int Y::X::_S_ios_base_init = 0;
int Y::X::X2::_S_ios_base_init = 0;
#endif

int _S_ios_base_init = 0;

void foo()
   {
     Y::X::_S_ios_base_init = 0;
   }
#endif

// Add this to force an error if name qualification for X::_S_ios_base_init is dropped.
// int _S_ios_base_init = 0;
