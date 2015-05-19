// This test code is a combination of pass1 and pass7, selected somewhat randomly
// from Jeremiah's test code of his inlining transformation from summer 2004.

int x = 0;

// Function it increment "x"
void incrementX()
   {
     x++;
   }

int foo()
   {
     int a = 0;
     while (a < 5)
        {
          ++a;
        }

     return a + 3;
   }

int main(int, char**)
   {
  // Two trival function calls to inline
     incrementX();
     incrementX();

  // Something more interesting to inline
  // for (; foo() < 7;)
     int i = 0;
     for (; foo() < 7; i++)
        {
          x++;
        }

     return x;
   }
