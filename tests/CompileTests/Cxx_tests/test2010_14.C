/*
example distilled from calc_key.c and ukey.h
UKey_getRandomSeed()
*/


// the trouble maker -->
typedef char foo_t(int, char*);
foo_t foo;
// <--

char foo(int x,char* y);
char foo(int x,char* y){
  // do something
  return 0;
}



