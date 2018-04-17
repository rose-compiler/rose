// This is valid C++ code, it is just a bit unusual.
// It also is different from where the try-catch are inside of "{}" body.
// ROSE currently handles this as though it were defined inside of "{}" body,
// the scoping rules are subtlely different for these cases.
// For more details see: http://stupefydeveloper.blogspot.com/2008/10/c-function-try-catch-block.html

// This will compile but is unparsed as "void bar1() { try {} catch (int) {} }"
void bar1() try {} catch (int) {}

// Note that this function "bar2()" has different semantics than "bar1()".
void bar2() { try {} catch (int) {} }


