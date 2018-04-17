
// Test try and catch exception handling

// Build a class to use with the throw and catch statements
class Overflow
   {
     Overflow(int i);
   };

void foo()
   {
  // Test the try statement
     try 
        {
          int i = 42;

       // Test the throw statement
          throw (i);
        }
  // Test the catch statement
     catch (Overflow xxx) 
        {
        }
   }
