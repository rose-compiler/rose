struct S {
  int x;
  int y;
};

int main() {
  S s1;
  int a;
  s1.x=5;
  s1.y=6;
  a=s1.y;
  a=a+s1.x+s1.y;
}
