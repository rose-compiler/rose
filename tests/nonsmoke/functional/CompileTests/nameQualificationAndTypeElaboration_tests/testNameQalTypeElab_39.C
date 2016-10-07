// number #39

typedef float numberType;

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
          void foo() {}

         ::numberType foo2 ();
   };
