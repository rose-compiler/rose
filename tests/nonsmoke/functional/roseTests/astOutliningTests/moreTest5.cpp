class A
   {
     public:
          int biz () const
             {
#pragma rose_outline
               biz();

               return 0;
             }
   };

int
main ()
   {
     A x;
  // x.biz();
     return 0;
   }
