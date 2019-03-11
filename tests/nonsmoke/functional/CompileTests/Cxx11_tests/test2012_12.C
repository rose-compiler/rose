// Lambda functions and expressions
// Main article: Anonymous function#C++

// C++11 provides the ability to create anonymous functions, called lambda functions.[10] 
// These are defined as follows:

void foobar()
   {
     [](int x, int y) { return x + y; };
   }

// The return type is implicit; it returns the type of the return expression (decltype(x+y)). 
// The return type of a lambda can be omitted as long as all return expressions return the 
// same type.
