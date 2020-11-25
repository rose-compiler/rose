extern "C" void foobar();
class A
   {
     public:
       // can't unparse friend with: extern "C"
          friend void foobar();
   };


