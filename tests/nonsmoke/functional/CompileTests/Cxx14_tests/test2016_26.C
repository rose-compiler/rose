// The [ [deprecated] ] attribute  

// The deprecated attribute allows marking an entity deprecated, which makes 
// it still legal to use but puts users on notice that use is discouraged and 
// may cause a warning message to be printed during compilation.

// The attribute may be applied to the declaration of a class, a typedef-name, 
// a variable, a non-static data member, a function, an enumeration, or a 
// template specialization.

// without a message:
int a [[deprecated]];

// with a message:
int b [[deprecated("message")]];

