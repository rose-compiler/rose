// This test code demonstrates an error in the unparsing of the addressing operators

template<class T> class SwigValueWrapper
   {
     T *tt;
     public:
       // operator T&() const;
          T *operator&() { return tt; }
   };

class X
   {
     public:
          int size();
   };

void foo()
   {
     SwigValueWrapper< X > result;
     int i;
     i = (&result)->size();
   }
