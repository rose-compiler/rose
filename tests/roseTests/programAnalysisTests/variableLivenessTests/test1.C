
int main()
   {
     /*
     int x = 42; // defined and live
     int g = 0; // live
     int y = x; // line in, not live out
     return 0;
     */

     int x;
     for (int i=0; i<10;++i) {
       x= 2;
       int y=x+1;
     }
     x=10;
   }
