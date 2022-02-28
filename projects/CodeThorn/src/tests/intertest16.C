// test inter-procedural propagation of constraints SAWYER_MESG(logger[TRACE])
// 1. with function arguments
// 2. on function return

void f(int y) {
  y=1; // delete value in y; this should not remove the constraints of main::y.
};

int g(int gy) {
  int local_gy=0;
  local_gy=gy;
  return local_gy; 
}

int h() {
  return 10;
}

void e() {
  int z;
  int y;
  int a;
  int arr[]={1,2,3};
  if(y==1) {
    f(y);
  } else {
    z=g(y);
  }
  a=z;
  //z=z; -- TODO
  y=z;
  z=2;
  h(); // return value must be descarded
  y=3;
  a=1;
}

void i() {
}

int main() {
  e();
  e();
  i();
  return 0;
}
