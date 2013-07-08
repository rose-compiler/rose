// This test code demonstrates an error in the unparsing of the addressing operators

class X
   {
     public:
          void foobar();
   };

class Y
   {
     public:
          X *operator&();
          X* operator -> ();

          X* result2;
   };

void foo()
   {
     Y result;

  // In this case the SgArrowOp is required.
     (&result)->foobar();

#if 1
     result.operator&()->foobar();
#endif

#if 1
  // In this case the SgArrowOp is supressed.
     result->foobar();
#endif

#if 1
     result.operator->()->foobar();
#endif

  // (&result)->result2->foobar();
     (&result)->foobar();
     result.result2->foobar();
   }
