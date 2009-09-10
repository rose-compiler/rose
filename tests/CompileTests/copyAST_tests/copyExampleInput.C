

#if 0
int x;
#endif

#if 0
int y;
int main( int x)
   {
     y = 1;
     return 0;
   }
#endif

#if 1
void foo();

// DQ (9/10/2009): GNU 4.3 reports: error: 'int main(int)' takes only zero or two arguments
// int main( int x)
int main( int argc, char* argv[])
   {
     foo();
   }
#endif
