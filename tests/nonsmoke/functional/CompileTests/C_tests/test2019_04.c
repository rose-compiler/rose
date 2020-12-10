
struct line 
   {
  // union xxxxxx
     union
        {
          unsigned serial;
        };
   };

#if 1
void foo()
   {
     struct line *b;
     b->serial;
   }
#endif
