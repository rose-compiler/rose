
class X
   {
     public:
          X(int x);

          X* ptr();
   };

   

void foo()
   {
     X* x;
  // for (X *i = x->ptr(), *j = x->ptr(); i != j; i = x->ptr(), j = x->ptr())
     for (X *i = x->ptr(), *j = x->ptr(); false; )
        {

        }
   }

