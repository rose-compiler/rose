// Mechanism to specify options to the preprocessor (global scope)
// Currently we don't handle where statements or indirect addressing (so assert that there are none)
TransformationAssertion globalScope0 (TransformationAssertion::NoWhereStatementTransformation,
                                      TransformationAssertion::NoIndirectAddressing);


int MAX_ITERATIONS = 0;

long
clock()
   {
     return 0L;
   }


int
functionNoTransformation ()
   {
  // TransformationAssertion assertion1 (TransformationAssertion::SkipTransformation);

     doubleArray A(10);

     long startClock = clock();
     for (int i = 0; i < MAX_ITERATIONS; i++)
        {
          TEST_STATEMENTS
        }
     long endClock = clock();
     long time = (endClock - startClock);

     return time;
   }

int
functionA ()
   {

     TEST_STATEMENTS

     return 0;
   }

int
functionB ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

     TEST_STATEMENTS


#if 0
     InternalIndex I,J,K,L;
     doubleArray A(10);
     doubleArray B(10);
     int m,n;

     A(I) = 0;
     A(I) = B(I-1);
     A(I+(m*2)) = B(I-(n+4)) + B(I+n);
     A(I) = A(I-1);
     A(I) = A(I-1) + A(I+1);
     A(I) = A(I-1) + A(I+1) + n;
#endif

     return 0;
   }


int
functionC ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
     TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

     TEST_STATEMENTS

     return 0;
   }


int
functionD ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
     TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

  // In addition to uniform stride we specify here that the stride is lenght 1 which permits
  // additional optimizations to the subscript computation (or the loop nest) to remove the stride
  // variable from the computations altogether.
     TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

     TEST_STATEMENTS

     return 0;
   }


int
functionE ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
  // TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
  // TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

  // In addition to uniform stride we specify here that the stride is lenght 1 which permits
  // additional optimizations to the subscript computation (or the loop nest) to remove the stride
  // variable from the computations altogether.
     TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

     TEST_STATEMENTS

     return 0;
   }


int
main ()
   {
     int timeWithoutTransformation = functionNoTransformation ();
     printf ("timeWithoutTransformation = %d \n",timeWithoutTransformation);

     return 0;
   }

