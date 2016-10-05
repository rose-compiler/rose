// These functions "foo()" unparse with name qualification by mistake.

class Domain {};

void foo(Domain X);

// The existence of this second function causes there to be multiple symbols for this function and triggers the name qualification.
void foo(Domain &X);
