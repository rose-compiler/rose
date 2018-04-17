// DQ (1/7/2007): added initalization of static variable so test name qualification.
namespace Y
   {
     class X
        {
       // public: static int _S_ios_base_init;

          void beforeDeclaration() 
             {
               ::Y::X::_S_ios_base_init = 0;
            // ::X::_S_ios_base_init = 0;
             }

          public: static int _S_ios_base_init;

        };
   }


// This should unparse as: int Y::X::_S_ios_base_init = 0;
int Y::X::_S_ios_base_init = 0;

// int X::_S_ios_base_init = 0;

