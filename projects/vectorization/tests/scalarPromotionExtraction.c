int main()
{
  float d = 5.f;
  float dim[16],dim2[16];
#pragma SIMD
  for(int i=0;i<16;++i)
  {
    dim[i] = d;
  }
#pragma SIMD
  for(int i=0;i<16;++i)
  {
    d = dim[i];
    dim2[i] = d;
  }
  d = d + 1;
}
