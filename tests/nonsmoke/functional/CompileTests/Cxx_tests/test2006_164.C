template < typename T > class A {};

struct Y
   {
  // typedef struct type_B typedefType_B;
     struct type_B *Bptr;

     A<struct type_B> N1;
   };

// This bug depends on no non-defining declaration existing.
// struct type_B* type_B_ptr;

// This is unparsed as "type_B* B_Y_type_B_ptr;" which is wrong, but 
// allows "A<type_B> N1;" to be compiled when in is improperly unparsed 
// from "A<Y::type_B> N1;"
// Y::type_B* B_Y_type_B_ptr;

// Without these defining declarations the template arguments assume the scope of the 
// undefined structures are global scope (which is wrong).  It should be the scope of
// the declaration and this appears to not be set properly. It should be the current 
// scope instead of the global scope as it is now!
// struct type_B {};
// struct Y::type_B {};

// This does not unparse properly ("A<type_B> N;", lacks qualified name for type "type_B")
// A<struct Y::type_B> N1;

// This is an error if there is no non-defining declaration in the global scope.
// A<type_B> N2;
