// With the new name mangling in place the use of variables 
// before they are defined is not working for classes.

#define DEMONSTRATE_BUG 1

int x;

class X
   {
     public:
          int y;

       // static 
          bool _S_initialized()
             { 
#if DEMONSTRATE_BUG
            // A variable declaration will be created for the first occurance of 
            // _S_ios_base_init but it will not be connected to the AST!
               return this->_S_ios_base_init;
#else
               return true;
#endif
             }

     private:
       // We need to have the previously used variable declaration be used here instead 
       // of a new one being created.  Either that or force the previous initialized name
       // (called prev_decl_item in Sage III)  to point to the correct declaration (this one).
       // static int _S_ios_base_init;
          int _S_ios_base_init;
   };

