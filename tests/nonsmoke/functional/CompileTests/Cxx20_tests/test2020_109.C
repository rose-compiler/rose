
// DQ (7/21/2020): Moved function calls into a function.
void foobar1()
   {
  // DQ (7/22/2020): Not enough is defined for this to be compilable code at present.

     for (/*each base or member subobject o of T*/)
        {
          if (auto cmp = lhs.o <=> rhs.o; cmp != 0) return cmp;
          return strong_ordering::equal; // converts to everything
        }
   }

