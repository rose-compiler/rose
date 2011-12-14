int main()
{
  double c, b;
  float a, d;
#pragma scop
  a = 0.6f;
  b = 120.7234234f;
  c = 4.0;
  d = 5;
#pragma endscop
}
