
#define MY_MACRO(factor,deriv,t) \
  {double my_x = 42.0 + t ; \
   (factor) = foo(-my_x*my_x) ; \
   (deriv)  = -2.0*(42.0)*my_x*(factor) ;}

double foo(double);

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

