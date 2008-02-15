// This example demonstrates the use of "GNU statement expressions"
// a gnu extension to C, which I guess we need to support since this
// is a bug submitted by IBM :-).

// The rules are: The last thing in the compound statement should be an expression 
// followed by a semicolon; the value of this subexpression serves as the value of 
// the entire construct. (If you use some other kind of statement last within the 
// braces, the construct has type void, and thus effectively no value.).

// Statement expressions are useful for macros such as:
#define maxint(a,b) ({int _a = (a), _b = (b); _a > _b ? _a : _b; })
// here the value of the expressions (a and b) are only computed once!
// without statement expressions the values would be computed twice.

// Example of use of variable declaration in statement expression
int foo();
int absolutValueOfFoo()
   {
     return ({ int y = foo (); int z; if (y > 0) z = y; else z = - y; z; });
   }

