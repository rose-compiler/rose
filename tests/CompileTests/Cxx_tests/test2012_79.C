void foobar();

class X
   {
     public:
          friend void foobar();
   };

#if 1
void foobar()
   {
   }
#endif
