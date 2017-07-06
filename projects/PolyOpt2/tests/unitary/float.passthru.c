
int main()
{
  double c;
  double b;
  float a;
  float d;
  
#pragma scop
  a = 0.6f;
  b = 120.7234234f;
  c = 4.0;
  d = 5;
  
#pragma endscop
}
