// Reduced example problem from test2004_85.C (which includes #include<string>).

int* foo()
   {
     int*  __last;
     int* __first;
     int __val;

     switch (1) // __last - __first)
        {
#if 0
          case 3:
               if (*__first == __val)
                    return __first;
               ++__first;
          case 2:
               if (*__first == __val)
                    return __first;
               ++__first;
#endif
#if 0
          case 1:
               if (*__first == __val)
                    return __first;
               ++__first;
#endif
          case 0:
          default:
               return __last;
        }
   }
