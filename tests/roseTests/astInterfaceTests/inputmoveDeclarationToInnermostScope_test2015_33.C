#define MY_MACRO(x) int x

void foo()
   {
     int x;
     MY_MACRO(y);
     if (1)
        {
           y = 8;
        }
   }

