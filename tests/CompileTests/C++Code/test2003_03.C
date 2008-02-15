// Strange cases to worry about when unparsing the CCP directives

#if 0
#if 1
#if 1
int x;
#endif
#endif

int main ()
   {
     x = 0;

     if (1)
        {
#if 1
        }
       else
        {
#endif
        }

     return x;
   }
#endif


#if 1
enum A {
    alpha,
#if 0
    beta
#else
#if 1
     delta,
#endif
#endif
 };
#endif


class foo
   {
#if 0
     int z;
   };
#else
#if 1
     int a;
#endif
   };
#endif

/*
void test ()
   {
#if 0
     int z;
   }
#else
#if 1
     int a;
#endif
   }
#endif
*/


