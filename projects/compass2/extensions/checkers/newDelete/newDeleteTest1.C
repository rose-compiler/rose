

class Y {
  int y;
};

void fail() {
  int x=2;
  // deleting array
  Y* m = new Y[5];
  delete m;

  // deleting NULL
  Y* n = 0;
  delete n;

  // deleting NULL
  Y* c ;
  if (x==5) {
    x=7;
    delete c;
  }
}

