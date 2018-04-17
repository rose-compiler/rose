// Simplest java example (note that class name must match filename).

public class pgc_2011_31
   {
  // This is the required "main" function.
     public static void main(String args[])
        {
          int x = 7;

       // Case of "for" statement with a statement as body (demonstrates use of SgBreakStatement).
       L1: for (x = 2; x < 1; x++)
               break L1;
        }
   }

