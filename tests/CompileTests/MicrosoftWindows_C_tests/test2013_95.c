
int x;

#if 1
/* Review this issues later (problem with MSVC support for un-named line directives). */
void foo()
   {
     int x;
     if (1)
        {
        }
# 864 "xxx"
     ;
   }
#endif
