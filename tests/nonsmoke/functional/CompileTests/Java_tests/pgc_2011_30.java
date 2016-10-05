// Simplest java example (note that class name must match filename).

public class pgc_2011_30
   {
  // This is the required "main" function.
     public static void main(String args[])
        {
          int x = 7;
       // int y = 8;

       // Case of "for" statement with SgBasicBlock as body.
       L1:  for (x = 2; x < 1; x++)
             {
            // Demonstrate "continue" statement.
               continue L1;
            // break;
             }
        }
   }

