// guaranteed copy elision through simplified value categories

struct X { int n; };

int k = X().n; // ok, X() prvalue is converted to xvalue

