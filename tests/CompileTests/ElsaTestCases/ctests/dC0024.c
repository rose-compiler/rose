// dC0024.c
// repeated type specifier keywords

typedef int int;

// sm: so far I've only seen 'int int' in the wild, though it is
// true that gcc accepts more than that
//typedef int float;              // !
//typedef unsigned int unsigned int;
