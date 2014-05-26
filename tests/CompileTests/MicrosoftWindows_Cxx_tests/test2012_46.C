int x;

#if 0
/* Review this issue later (problem with MSVC support for reference to compiler defined implicit copy constructor). */
class X
   {
   };


X foo()
   {
     X object1;

  // Should be unparsed as: "X object2 = X();"  but is equivalent as "X object2();"
     X object2 = X();

     X object3 = object1;
   }
#endif

