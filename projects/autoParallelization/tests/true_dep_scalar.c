// ! This is an example to demonstrate a possible bug 
//  in Qing' dependence analysis: 
//   scalar to array dependences are reported
// According to her:  
//   A scalar dep is simply the dependence between two scalar variables.
//   There is no dependence between a scalar variable and an array variable. 
//   The GlobalDep function simply computes dependences between two scalar 
//   variable references (to the same variable)
//   inside a loop, and the scalar variable is not considered private.
void foo()
{
  int i;
  int a[100];

  int temp, t2; 
  temp = 1;
  t2 = temp; 
#if 1  
  for (i=0;i<99;i++)
  {
    temp = a [i];
    a[i+1]+=temp + 1;
  }  
#endif  
}  
