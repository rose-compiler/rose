// number #33

#if 0
class myNumberType
   {
     public:
         void foobar();
   };

typedef ::myNumberType myNumberType;
typedef myNumberType numberType;
#else
typedef float numberType;
#endif

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
       // Example of where "::" is significant, without it numberType 
       // will be an "int" instead of a "myNumberType"
          typedef ::numberType numberType;
   };
