struct line 
   {
  // union xxxxxx
     union
        {
          unsigned serial;
        };
   };

#if 0
void foo()
   {
     struct line *b;
     b->serial;
   }
#endif
