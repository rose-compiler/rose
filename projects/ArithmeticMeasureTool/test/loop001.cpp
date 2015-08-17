// simplest test
//
void foo()
{
  float r, a, s,d;
  int i; 
#pragma aitool fp_plus(2) fp_multiply(1) fp_divide(1)
  for (i=0; i<100; i++)  
  {
    r +=(a+s)/(2*d);    
  }
}

void bar(int i, int j, float fi, float fj)
{
#pragma aitool fp_total(0)  
  for(int i=0; i<100; i++)
    j = i*i;

#pragma aitool fp_total(0)  
  for(int i=0; i<100; i++)
    fj = fj+ fi *fi -fi/0.5;

}
