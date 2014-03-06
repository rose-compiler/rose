//! Simplest pragma to define a resilience region

void foo()
{
  int i;
L1:
#pragma failsafe region
  {
L2:
    i++;
  }

  i--;
}

