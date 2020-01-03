// This is an example test code similar to test2019_518a.C
// It demonstrates a defining class declaration in typedef instead of a variable declaration.

#if 0
class X
   {
     public:
          X(int ii)
             { }
   } x(51);
#endif

typedef 
   enum X 
      { 
        LAST_ELEMENT
      } X_type;

X_type xxx(LAST_ELEMENT);

