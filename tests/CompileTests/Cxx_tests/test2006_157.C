
struct A { struct B{}; };

void foobar()
   {
     struct A { struct D{}; };

     typedef ::A::B x3_type;
     typedef ::A x4_type;

  // This does not work properly (unparses as "typedef struct D x5_type;")
     typedef A::D   x5_type;
   }
