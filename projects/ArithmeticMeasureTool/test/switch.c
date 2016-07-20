float foo(int f, float a, float b)
{
  float c;
#pragma aitool fp_plus(1) fp_multiply(1)
  switch(f)
  {
    case 1:
       c = a+b; 
       c = c*b; 
       break;
    case 2:
       c = a-b; 
       break;
   default:
       break;
  }
  return c; 
}
    
