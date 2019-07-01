struct X;

// Original code: int (X::* ptfptr) (int);
// Unparsed as:   int (X::*ptfptr)(int (X::*);
int (X::* ptfptr) (int);

