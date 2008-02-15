// g0029.cc
// __complex__
// from gcc-3.4.3 <complex> header

typedef __complex__ float _ComplexT;

//ERROR(1): typedef __complex__ /*missing*/ something;

void f()
{
  _ComplexT c1, c2;
  float f1, f2;

  // read parts
  f1 = __real__ c1;
  f1 = __imag__ c1;
  
  //ERROR(2): __real__ f1;
               
  // write parts
  __real__ c1 = f1;
  __imag__ c1 = f1;
                
  // read+write
  __real__ c1 += f1;

  // arith on complex
  c1 = c2;
  c1 = c1 + c2;
                     
  // mixed arith
  c1 = c1 * f1;
  c1 *= f1;

  c1 = c1 / f1;
  c2 /= f1;
}


