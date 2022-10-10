/*
MS: interesting test case: without calling context function f2
is analyzed twice. 1st time after 1st call of f1,
2nd time (due to imprecision) after the 2nd call of f1 because the state
also flows to the first call of f1 and then again the call of f2.
This is an execution path which can never be taken, but is reported
by the analysis (if not calling context is used).
*/
int f1(int x) {
  if(x==1) {
    x=2;
    return x;
  } else {
    x=3;
    return x+1;
  }
}
int f2(int x) {
  x=6;
  return 2;
}

int main() {
  int x=1;
  f1(x);
  f2(x);
  f1(x);
  return 0;
}
