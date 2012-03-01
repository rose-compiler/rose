#ifdef PRINT
#include <cstdio>
#else
#define NULL ((List*)0)
#endif

class List {
public:
  List(int _d) : n(NULL), d(_d) {}

  void print() {
    #ifdef PRINT
    printf("%d", d);
    if (n) {
      printf(", ");
      n->print();
    }
    else {
      printf("\n");
    }
    #endif
  }

  List *n;
  int d;
};
