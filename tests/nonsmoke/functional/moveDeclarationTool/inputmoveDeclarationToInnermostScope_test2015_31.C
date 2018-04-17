
#define MY_MACRO(x) x+x

void foo()
   {
     int x;
     int y;
     if (1)
        {
          x = 4;
        }

     MY_MACRO(y);
   }

