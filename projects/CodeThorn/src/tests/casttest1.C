int main() {
  float f;
  double d=0.0;
  double b[5];
  double c[5][6][7];
  f=d;
  d=f;
  f=d+1.0;
  d=f+1.0f;
  d=f+1.0;
  b[2]=3.0;
  c[1][2][3]=4.0; 
  d += 6;
  b[2] += 5;
  c[1][2][3] += 5;
  return 0;
}
