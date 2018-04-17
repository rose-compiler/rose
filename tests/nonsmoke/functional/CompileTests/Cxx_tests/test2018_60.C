namespace Y
   {
   }

void foobar()
   {
  // Without global qualification.
  // Note that this unparses as "namespace Y = ::Y;" which might be an error, depending upon the C++ semantics.
     namespace Y = Y;
   }

