
struct line 
   {
     union
        {
          unsigned serial;
        };
   };

void foo()
   {
     struct line *b;
     b->serial;
   }
