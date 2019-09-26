/*
Original code:
extern "C" int g_(int i)
   {
     extern int f_(int i);
     f_(i);
     return 2;
   }

extern "C" int f_(int i);
*/


extern "C" int g_(int i)
   {
     extern int f_(int i);
     f_(i);
     return 2;
   }

extern "C" int f_(int i);



