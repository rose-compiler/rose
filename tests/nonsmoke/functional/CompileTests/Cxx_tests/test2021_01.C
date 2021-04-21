
// This is a copy of test2007_158.C except for the added statement "x = 43;" (and the comment) below.

void foo()
   {
#if 1
     if (int x = 42)
        {
       // This is required to pass the tests in 
          x = 43;
        }
       else
        {
        }
#endif
#if 1
     switch (int x = 7)
        {
        }
#endif
#if 1
     while(int x = 7)
        {
        }
#endif
#if 1
     for (int i=1; i < 10; i++)
        {
        }
#endif
#if 1
     for (int x=1,y=2; int test=3; x++,y++)
        {
        }
#endif
   }
