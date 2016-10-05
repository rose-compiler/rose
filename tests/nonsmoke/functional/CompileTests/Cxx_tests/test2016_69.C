#include <stdio.h>

// class X0 {};
class X0
   { 
     public: 
       // Note that we need a virtual member function to have the polymophism required for the dynamic cast.
          virtual void foobar() {}
   };

class X : public X0 {};

class Y : public X {};

int main()
   {
  // Y* dynamic_yPtr = (Y*) new X0;
     Y* dynamic_yPtr = static_cast<Y*>(new X0);

  // This is a base class cast (will not be considered to be a dynamic cast.
     X*  xPtr3 = dynamic_cast<X*>(dynamic_yPtr);
     X0* xPtr4 = dynamic_cast<X0*>(xPtr3);

     if (xPtr4 != NULL)
       {
         printf ("Valid dynamic cast! (xPtr4 != NULL) \n");
       }
     else
       {
         printf ("Invalid dynamic cast! (xPtr4 != NULL) \n");
       }

     if (dynamic_cast<X*>(dynamic_yPtr) != NULL)
       {
         printf ("Valid dynamic cast! (dynamic_cast<X*>(dynamic_yPtr) != NULL) \n");
       }
     else
       {
         printf ("Invalid dynamic cast! (dynamic_cast<X*>(dynamic_yPtr) != NULL) \n");
       }

     if (dynamic_cast<X*>(xPtr4) != NULL)
       {
         printf ("Valid dynamic cast! (dynamic_cast<X*>(xPtr4) != NULL) \n");
       }
     else
       {
         printf ("Invalid dynamic cast!  (dynamic_cast<X*>(xPtr4) != NULL) \n");
       }

     printf ("Terminated Normally! \n");

     return 0;
   }

