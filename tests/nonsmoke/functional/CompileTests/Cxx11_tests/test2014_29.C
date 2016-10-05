// This is a test code from Reed, it is an issue for the deleteAST function.
// In this case a template declaration is generated as part of the template parameter
// and the template declaration is expected to have an associated symbol
// (when the call the search_for_symbol_from_symbol_table(), which has
// an assertion error).

// This is an argument for all declarations to have a hidden scope to hold
// symbols generally.  Similar to the argument for function declarations to
// have a scope where symbols associated with function parameters could
// be held (such as in "void foo(int n, int array[n] a);").

// template< typename T1 > struct T { };
// template< class T1 > struct T { };
template< template< class T1 > class T2 , class T3 > struct T { };
