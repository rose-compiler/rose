// Simplest java example (note that class name must match filename).

public class test2011_15
   {
  // This is the required "main" function.
     public static void main(String args[])
        {
          int x = 7;

       // This causes the block to be empty, might be an issue with the error detection.
       // x == 0;

       // if (x == 10) if (x == 11) x = 12; else x = 13; else x = 14;
          if (x == 20) if (x == 21) x = 22; else x = 23; else if (x == 24) x = 25; else x = 26;
        }
   }
