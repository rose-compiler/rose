// This test code fails because the cast operator is not explicit and 
// the case to a reference type is not translated or maybe output correcting.
// It might point ot a reference cast being missing from the ROSE IR, or 
// it might be that we need to unparse this better.

template<class T> class SwigValueWrapper {};

class X {};

void foo()
   {
  // Case using non-nested template class (any class type)
     SwigValueWrapper< X > X_result;
     X *X_resultptr;

  // This unparses as: "X_resultptr = (new X (X_result));" missing the explicit cast to a reference.
     X_resultptr = new X ((X&)X_result);
   }

