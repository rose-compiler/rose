#include "UserDefList.h"

void foo ( int a ) {
  printf ("a = %d \n",a);
}

int main() {
  list l;
  for (list::iterator i = l.begin(); i != l.end(); i++) {
    foo(*i);
    for (list::iterator i = l.begin(); i != l.end(); i++) {
      foo(5);
    }
    foo(*i);
    int k;
  } 
  return 0;
}









