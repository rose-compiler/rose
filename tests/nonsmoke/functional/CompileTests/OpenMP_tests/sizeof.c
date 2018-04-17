// Liao, 11/17/2009
// Test SgSizeOfOp::replace_expression()
// Distilled from spec_omp2001/benchspec/OMPM2001/332.ammp_m/atoms.c
int atom()
{
   int serial;
#pragma omp parallel
   {
     int i =sizeof(serial);
     serial = i; 
    }
  return serial;
}

