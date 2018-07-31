struct S {
  int x;
  int y;
};

int main() {
  S s1;
  int a;
  a=s1.y; // intentionally uninitialized
  s1.x=5;
  a=a+s1.x+s1.y;
}
