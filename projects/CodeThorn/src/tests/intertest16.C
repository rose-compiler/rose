// test inter-procedural propagation of constraints 
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

int main() {
  int z;
  int y;
  int a;
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
  return 0;
}
