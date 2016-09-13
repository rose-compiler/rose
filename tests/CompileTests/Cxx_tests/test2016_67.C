class X 
   { 
     public: 
       // Note that we need a virtual member function to have the polymophism required for the dynamic cast.
          virtual void foobar(); 
   };

class Y : public X {};

X* xPtr = 0L;

Y* yPtr = dynamic_cast<Y*>(xPtr);

