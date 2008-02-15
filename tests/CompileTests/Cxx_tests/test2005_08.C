// Original Code:
// SP< Vector2d > expr((new Vector2d((*(self->object))->operator+(*(a0_0.bp())))));
// Generated Code:
// class SP< Geometry::Vector2d  > expr((new Geometry::Vector2d (((*self -> object)->+*a0_0.bp()))));

class X
   {
     public:
          X & operator+ ( const X & x ); // { return *this; }
          X *operator->() const;         // { return (X*) this; }
   };

int foo()
   {
      X x;
#if 1
   // All three of these statements are identical (different syntax for the same function calls) 
   // They generate an error "error: parse error before `+' token"
      x->operator+(x);
      (*(x.operator->())).operator+(x);
      (*(x.operator->())) + (x);
#endif

   // This is similar to just saying "x;" but with a function call to accomplish it!
      (*x.operator->());  // This works properly
#if 1
      x.operator->();
   // x->;  // note that this is not allowed in C++!
#endif
   }

