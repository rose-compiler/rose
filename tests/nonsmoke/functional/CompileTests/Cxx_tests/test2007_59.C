// Example code used as Binary Example for ROSE
typedef struct mystruct {
  int x;
} mystruct_t;

void f2() {
  mystruct_t my;
  my.x=2; 
  double y=3; 
  y = my.x + y;
}

void f1() {
  f2();
}

int main() {
  f1();
  f2();
  return 0;
}
