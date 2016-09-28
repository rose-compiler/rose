namespace C
   {
  // forward declaration of D
     class D;
     void foo();

  // name qualification not allowed here
  // void ::C::foo() {}
   }


#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )

// Class definition of D with scope in C but defined in global scope
class ::C::D {};

#else
  #warning "Case not tested for version 3.4 and higher."
#endif

void ::C::foo() {}

class D
   {
   };

