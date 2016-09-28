// number #30

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
