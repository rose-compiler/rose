// The example extracted from GeorgeIterated1996, Figure 2.
// used to verify the correctness of liveness analysis
// Liao 6/12/2012
void foo (int *mem, int j, int k, int bound)
{
  int g, h, f, e, m, b, c, d;
#pragma rose [LiveVarsLattice:liveVars=[mem,j,k,bound]]
  g = mem [j +12];
#pragma rose [LiveVarsLattice:liveVars=[mem,j,k,bound,g]]
  h = k -1;
#pragma rose [LiveVarsLattice:liveVars=[mem,j,bound,g,h]]
  f = g*h;
#pragma rose [LiveVarsLattice:liveVars=[mem,j,bound,f]]
  e = mem [j+8];
#pragma rose [LiveVarsLattice:liveVars=[mem,j,bound,f,e]]
  m = mem [j+16];
#pragma rose [LiveVarsLattice:liveVars=[mem,bound,f,e,m]]
  b = mem [f];
#pragma rose [LiveVarsLattice:liveVars=[bound,e,m,b]]
  c = e +8;
#pragma rose [LiveVarsLattice:liveVars=[bound,m,b,c]]
label1:
#pragma rose [LiveVarsLattice:liveVars=[bound,m,b,c]]
  d =c;
#pragma rose [LiveVarsLattice:liveVars=[bound,m,b,c]]
  k = m +4;
#pragma rose [LiveVarsLattice:liveVars=[k,bound,m,b,c]]
  j = b;
#pragma rose [LiveVarsLattice:liveVars=[k,bound,m,b,c]]
  if (k < bound)
#pragma rose [LiveVarsLattice:liveVars=[bound,m,b,c]]
    goto label1;
}
