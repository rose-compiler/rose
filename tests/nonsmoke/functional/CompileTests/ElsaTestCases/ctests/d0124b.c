// see elsa/in/d0124.cc for contrast; this passes because string
// literals are 'char (nonconst) []' in C
char *a = "hello";
char const *b = "hello";
