// This is a simpler version of test2011_29.C

#if 0
double divide(double top, double bottom) 
   {
     int divide_by_zero = 1;

     if (bottom == 0.0) 
        {
          throw divide_by_zero;
        }
       else 
        {
          return top/bottom;
        }
   }
#endif

int main() 
   {
  // double result;

     try 
        {
       // result = 0.0; // divide(12.3, 0.0);
        }
  // Bug in use of named exception (does not unparse correctly)
     catch (int i)
        {

        }
   }

