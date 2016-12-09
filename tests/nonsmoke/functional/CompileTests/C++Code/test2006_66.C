int
main() 
   {
     double result;

     try 
        {
          result = 3.14;
        }

  // Bug in use of named exception (does not unparse correctly)
     catch(int i)
        {
          result = 2.71;
        }
   }
