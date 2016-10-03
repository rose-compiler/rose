// Original Code:
// SP< Vector2d > expr((new Vector2d((*(self->object))->operator+(*(a0_0.bp())))));
// Generated Code:
// class SP< Geometry::Vector2d  > expr((new Geometry::Vector2d (((*self -> object)->+*a0_0.bp()))));

class X
   {
     public:
          X & operator+ ( const X & x ); // { return *this; }
          X * operator->() const;         // { return (X*) this; }
          X * foobar();
   };

int foo()
   {
      X x;
#if 1
   // All three of these statements are identical (different syntax for the same function calls) 
   // They generate an error "error: parse error before `+' token"
//    x->operator+(x);
//    (*(x.operator->())).operator+(x);
//    (*(x.operator->())) + (x);
//    (*(x.operator->()));
      *(x.operator->());
#endif

#if 0
   // This is similar to just saying "x;" but with a function call to accomplish it!
      (*x.operator->());  // This works properly
#endif
#if 0
      x.operator->();
   // x->;  // note that this is not allowed in C++!
#endif

#if 1
      *x.operator->();

   // DQ (4/9/2013): This fails because of the precedence issue of member functions that should have the asociated presendence of their overloaded operators).
   // x.operator+(x);

      *x.foobar();
#endif
   }

