
int global;

class foo {

 public:
  void ptrMod(int *x) { *x = 2; }
  void refMod(int &x) { x = 2; }
  void globalMod() { int local; local = 2; global = local; }

};

int main(int argc, char **argv)
{
  foo f;
  int x, y;

  f.ptrMod(&x);
  f.refMod(y);
  f.globalMod();

  return 0;
}
