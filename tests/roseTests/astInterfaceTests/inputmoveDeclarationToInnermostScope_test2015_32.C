#define MY_MACRO(x) x+x

void foo()
   {
     int x;
     int y;
     if (1)
        {
          x = MY_MACRO(y);
        }

     MY_MACRO(y);
   }

