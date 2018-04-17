#define MACRO(a, b, c) c = a + b; c = a * b;

// void MACRO(int,int,int);

void foobar() 
   {
     int d = 7;
     int e,f;

     if (true) 
        {
          MACRO(d, 42, e)
        }
   }

