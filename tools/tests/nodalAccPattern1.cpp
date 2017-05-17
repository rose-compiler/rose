//Test case for nodal accumulation pattern
// 
//
// A positive test case: with a pattern to be matched
void foo(double* x, int jp, int kp, int begin, int end, double rh1)
{
   //Condition 1: pointer declaration, 4 or 8 pointers
   double * x1, *x2, *x3, *x4; 

   //Condition 2:  pointer initialization, using other pointers on rhs
   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   //Condition 3:  A regular loop or a RAJA loop
   for (int i = begin; i< end; i++)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() 
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } 
}
// Another positive test case: with empty statement ; in the sequence
// One example input lab app. uses macro expansion, which introduces ;; . 
void foo0(double* x, int jp, int kp, int begin, int end, double rh1)
{
   //Condition 1: pointer declaration, 4 or 8 pointers
   double * x1, *x2, *x3, *x4; 

   //Condition 2:  pointer initialization, using other pointers on rhs
   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   //Condition 3:  A regular loop or a RAJA loop
   for (int i = begin; i< end; i++)
   {
      // Condition 4: accumulation pattern: lhs accum-op rhs
      // lhs : array element access x[i]: x is pointer type, i is loop index 
      // rhs: a scalar double type
      // accum-op:   +=, -=, *=, /=, MIN (), MAX() , 
      x1[i] += rh1; ;  // extra ; 
      x2[i] -= rh1; 
      x3[i] *= rh1; ; // extra ; 
      x4[i] /= rh1; 
   } 
}


// negative test case: the tool should not find the patterns. 
// -------------------------------------------
void foo1(double* x, int jp, int kp, int begin, int end, double rh1)
{
   //Condition 1: pointer declaration, 4 or 8 pointers
   double * x1, *x2, *x3, *x4; 

   //Condition 2:  pointer initialization, using other pointers on rhs
   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   //Condition 3:  A regular loop or a RAJA loop
   for (int i = begin; i< end; i++)
   {
      x1[i] = rh1; // not accumulation pattern
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } 
}


// Negative test: Only 3 statements in a sequence
// -------------------------------------------
void foo2(double* x, int jp, int kp, int begin, int end, double rh1)
{
   double * x1, *x2, *x3; 

   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 

   for (int i = begin; i< end; i++)
   {
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
   } 
}


// Negative test: not double type
// -------------------------------------------
void foo3(int * x, int jp, int kp, int begin, int end, int rh1)
{
   int * x1, *x2, *x3, *x4; 

   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   for (int i = begin; i< end; i++)
   {
      x1[i] += rh1; 
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } 
}

// tentative negative test case: loop without init statment 
// -------------------------------------------
void foo4(double* x, int jp, int kp, int begin, int end, double rh1)
{
   double * x1, *x2, *x3, *x4; 

   x1 = x;
   x2 = x +1; 
   x3 = x1 + jp; 
   x4 = x1 + kp; 

   int i=begin; 
   for (; i< end; i++)
   {
      x1[i] += rh1; // not accumulation pattern
      x2[i] -= rh1; 
      x3[i] *= rh1; 
      x4[i] /= rh1; 
   } 
}


