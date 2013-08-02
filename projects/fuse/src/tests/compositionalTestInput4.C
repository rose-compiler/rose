// This test evaluates the ability of LiveDeadMemAnalysis to identify complex memory locations as dead.
// This variant can detect that memory locations array2D[1][1], array2D[5][1] and array2D[6][1] are live
// because the return statement returns all of them. However, to do so, the OrthogonalArrayAnalysis needs
// to be preceded by [OrthogonalArrayAnalysis, ConstantPropagationAnalysis, OrthogonalArrayAnalysis] to
// enable it to interpret expressions such as array2D[array2D[1][1]][1].

#pragma fuse lc(ccs, oa, cp, oa, ld, oa, cp)
// ld, oa, 
void CompDebugAssert(bool);
int global;

int main()
{
  int array2D[10000][10000];

  array2D[1][2] = 5;
  CompDebugAssert(array2D[1][2]==5);
  
  array2D[array2D[1][2]][3] = 6;
  CompDebugAssert(array2D[5][3]==6);
  
  return array2D[1][2]+array2D[array2D[1][2]][3];
}
