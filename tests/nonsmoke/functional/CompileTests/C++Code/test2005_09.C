// This code demonstrates an unparser error where the division operator "operator/()" 
// and the derference operator "(*point)" conspire with the operator presedence
// to turn "operator/() (*ptr)" into "/*ptr" :-).

// Original code:
// SP< Vector2d > expr((new Vector2d((*(self->object))->operator/((*(a0_0.bp()))))));
// Generated Code:
// class SP< Geometry::Vector2d  > expr((new Geometry::Vector2d (((*(*self -> object).operator->())/*a0_0.bp()))));


class X
   {
     public:
          X & operator/ ( const X & x ); // { return *this; }
   };

int foo()
   {
      X x, *xptr;

   // This generates "x/*xptr;" which is the start of a comment!  Disaster follows quickly!
      x.operator/(*xptr);
   }

