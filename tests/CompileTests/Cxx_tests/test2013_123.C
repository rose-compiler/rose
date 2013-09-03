// The bug is present for classes (not just templates).
class X
   {
     public:
          bool operator!();
   };

void NEW_write()
   {
     bool a;
     if (!a)
        {
        }

     X x;
     if (!x)
        {
        }
   }

