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
   class X 
      { 
        public:
             X(int ii) { }
      } X_type;

X_type xxx(42);

