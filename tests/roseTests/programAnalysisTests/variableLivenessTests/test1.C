
int main()
   {
#if 0
     int x = 42; // defined and live
     int y = x; // line in, not live out
     int g = 5;
     x = y;
     return x;
#endif

#if 1

     int x;
     for (int i=0; i<10;++i) {
       x= 2;
       int y=x+1;
     }
     x=5;
#endif
   }
