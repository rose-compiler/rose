double divide(double top, double bottom) 
   {
     int divide_by_zero = 1;

     if(bottom == 0.0) 
        {
          throw divide_by_zero;
        }
       else 
        {
          return top/bottom;
        }
   }

int main() 
   {
     double result;

     try 
        {
          result = divide(12.3, 0.0);
       // printf("The result is %g.\n", result);
        }
  // Bug in use of named exception (does not unparse correctly)
     catch(int i)
        {
       // printf("ERROR:  Divide by zero.\n\n");
        }
   }

