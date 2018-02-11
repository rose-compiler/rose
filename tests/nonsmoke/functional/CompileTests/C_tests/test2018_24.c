
// This is why we need the "*" syntax (because "var" has not been declared yet).
void foobar (int size, double array[][*]);

int var = 42;

// BUG: this is unparsed as:
//    void foobar(int size,double array[][])
void foobar (int size, double array[][size+var])
   {
   }
