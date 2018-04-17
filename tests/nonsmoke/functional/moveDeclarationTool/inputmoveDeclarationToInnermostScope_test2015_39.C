#define MY_MACRO(a, b, c) c = a + b; c = a * b;

// void MACRO(int,int,int);

void foobar() 
   {
     int d = 7;
     int e,f;

     if (true) 
        {
          MY_MACRO(d, 42, e);
        }
   }

