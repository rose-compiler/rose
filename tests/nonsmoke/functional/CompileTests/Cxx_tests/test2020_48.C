// An even simpler example appears when a functional cast is intended to convert an expression for initializing a variable or passing to a constructor parameter

void f(double adouble) {
  int i(int(adouble));
}

// In this case, the parentheses around adouble are superfluous and the declaration of i is again a function declaration equivalent to the following

// takes an integer and returns an integer
int i(int adouble);

// To disambiguate this in favour of a variable declaration, the same technique can be used as for the first case above. Another solution is to use the cast notation:

// declares a variable called 'i'
int i((int) adouble);

// Or also to use a named cast:

// declares a variable called 'i'
int i(static_cast<int>(adouble));
