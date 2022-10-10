int f() {
  return 1;
}
int g() {
  return 2;
}

int h() {
  int x=f()+g();
  if(x>1) {
    x+=10;
  }
  return x;
}
