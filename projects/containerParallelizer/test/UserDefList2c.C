#include "UserDefList.h"

void foo ( int a ) {
  printf ("a = %d \n",a);
}

int main() {
  list l;
  int x;
  for (list::iterator i = l.begin(); i != l.end(); i++) {
    foo(*i);
    int t;
    foo(*i);
    for (list::iterator i = l.begin(); i != l.end(); i++) {
      foo(*i);
      for (list::iterator i = l.begin(); i != l.end(); i++) {
	x=5;
	while(x<10) { 
	  foo(*i); 
	  while(x<10) { 
	    foo(x);
	    x++;
	  }
	  foo(*i);
	}
	foo(*i);
      }
    }
  }
  for (list::iterator i = l.begin(); i != l.end(); i++) {
    foo(*i);
  }
  return 0;
}









