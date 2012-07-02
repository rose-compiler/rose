/*Constant propagation test case

Liao 7/1/2012
*/

void foo()
{
 int a = 2;
 int b = 3;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 a:ConstantPropagationLattice:[level:constantValue,val=2] \
 b:ConstantPropagationLattice:[level:constantValue,val=3] \
 ]
 int c = a+b+4;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 c:ConstantPropagationLattice:[level:constantValue,val=9] \
 ]
 int d =c;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 d:ConstantPropagationLattice:[level:constantValue,val=9] \
 ]
  a = d+1;
}
