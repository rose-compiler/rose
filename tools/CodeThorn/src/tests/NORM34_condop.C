int f() {
  return 1;
}
int g() {
  return 2;
}
int h() {
  return 4;
}

int main() {
  // single cond op
  int res=0;
  int x=1;
  res=(g()==2)?2+x:16+x;

  // nested cond op [ not working yet, varNr=0 !
  //if( ( f()==1?((g()==2)?2:16):h() )==2 ) {
  //   res=8;
  //}
  return 0;
}
