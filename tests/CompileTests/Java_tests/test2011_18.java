// Simplest java example (note that class name must match filename).

public class test2011_18
   {
  // This is the required "main" function.
     public static void main(String args[])
        {
          int x = 7;
          int y = 8;

          x += y;
          x -= y;
          x /= y;
          x *= y;
          x |= y;
          x &= y;
          x ^= y;
          x >>= y;
          x <<= y;
          x %= y;
          x >>>= y;

       // These are not supported in Java.
       // x ||= y;
       // x &&= y;
        }
   }
