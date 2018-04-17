
int global;

class foo {

 public:
  void ptrMod(int *x) { *x = 2; }
  void refMod(int &x) { x = 2; }
  void globalMod() { int local; local = 2; global = local; }

};

class bar {

 public:
  void ptrMod(int *x) { ; }
  void refMod(int &x) { ; }
  void globalMod() { int local; local = 2; }

};

int main(int argc, char **argv)
{
  foo f;
  bar b;
  int x, y;

  f.ptrMod(&x);
  f.refMod(y);
  f.globalMod();

  b.ptrMod(&x);
  b.refMod(y);
  b.globalMod();

  return 0;
}

