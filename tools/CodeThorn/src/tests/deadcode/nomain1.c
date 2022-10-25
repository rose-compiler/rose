int e(int x) {
  return x+1;
}

float f(float x) {
  if(1) {
    x+=1.0;
    return x;
  } else {
    x=x+1.0;
    return x;
  }
}

float g(int x) {
  x=f(x);
  return (float)x;
}

void h() {
  e(1);
  f(2.0);
}
