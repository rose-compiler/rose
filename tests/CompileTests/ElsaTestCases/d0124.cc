// see elsa/in/c/d0124b.c for contrast; string literals are 'char
// const []' in C++; this only passes because of a special conversion
// that drops the const on string literals; Scott puts it thus: "See
// cppstd 4.2 para 2.  There is a special exception for converting a
// string literal to char*.  This is the reason for the existence of
// SE_STRINGLIT."
char *a = "hello";
char const *b = "hello";
