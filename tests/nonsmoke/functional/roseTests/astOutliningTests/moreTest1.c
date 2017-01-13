/* This code tests two things:
 *  1) for c code, extern "C" should not be inserted
 *  2) the bug: int &a = *ap__;
 */
void foo()
{
  int i,sum=0;
#pragma rose_outline
  for (i=0;i<100;i++)
  {
    sum +=i;
  }
}

