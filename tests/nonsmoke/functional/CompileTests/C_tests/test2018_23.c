
// This code demonstrates that the function type for the prototype must be unparsed 
// differently than the function type for the defining declaration.  We can't use
// the same function type in unparsing the defining function. Or at least we can't
// unparse it can the same.

void foobar (int size, double array[][size+1]);

// BUG: this is unparsed as:
//    void foobar(int size,double array[][])
void foobar (int size, double array[][size+1])
   {
   }
