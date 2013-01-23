class X
   {
     public:
       // This is unparse without the "static" keyword (so it is an error).
          static void atomic_add_dispatch(X* __mem) {}
   };


class Y
   {
     public:
          static X var;

          void
          foo ()
             {
                X::atomic_add_dispatch(&var);
             }
   };

