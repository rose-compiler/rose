// cc.in48
// experimenting with declarator notation printing

// essentially, declarators have three kinds of type
// constructors:
//   pointer    *       prefix
//   function   (...)   postfix
//   array      [n]     postfix
// parsing ambiguity arises because of the combination of
// prefix and postfix syntax, and is resolved by always
// considering the postfix syntax to bind more tightly

int a;

int *b;

int **c;

int d[2];

// array of 2 pointers to ints
int *e[2];
int *(f[2]);

// pointer to an array of 2 ints
int (*g)[2];

int h();

// function returning pointer to int
int *i();
int *(j());

// pointer to function returning int
int (*k)();

// function accepting an integer
int f1(int n);
class S;
int f2(int S);

// function accepting a pointer to a function which returns int
int l(int (*)());
int m(int (*n)());

// function accepting a function which returns int
int p(int (q)());
int f3(int (S));        // ambiguous..

// function accepting a function which returns a function (!)
// dsw: nope
//  int q(int (S)());       // ayyyeeee!!
//  int r(int ()());


