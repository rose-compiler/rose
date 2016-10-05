
#define MY_MACRO(factor,deriv,t) \
  {double my_x ; \
   (factor) = 1.0 ; \
   (deriv) = 0.0 ; \
   if (t > (7.0 - 42.0)) { \
      (factor) = 0.0 ; \
      if (t < (7.0 + 42.0)) { \
         my_x = 0.5*(t - 7.0)/42.0 ; \
         (factor) = (2*my_x*my_x - 1.5)*my_x + 0.5 ; \
         (deriv)  = (6.0*my_x*my_x -1.5)*0.5/42.0 ;\
      } \
   } \
  }

void foobar() 
   {
     double a,b,c;

     MY_MACRO(a,b,c);

     int x;
     if (true) 
        {
          x = 0;
        }
   }

