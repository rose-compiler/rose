int f(int x) {
  return x+1;
}

float g(float x) {
  if(true) {
    x+=1.0;
    return x;
  } else {
    x=x+1.0;
    return x;
  }
}

template<typename T>
T h(T x) {
  if(true) {
    x+=5.0;
    return x;
  } else {
    x=x+6.0;
    return x;
  }
}

