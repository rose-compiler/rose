/*Constant propagation test case

Liao 7/1/2012
*/

void foo()
{
 int a, b, c, d;
 a = 2;
 b = 3;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 a:ConstantPropagationLattice:[level:constantValue,val=2] \
 b:ConstantPropagationLattice:[level:constantValue,val=3] \
 ]
 c = a+b+4;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 c:ConstantPropagationLattice:[level:constantValue,val=9] \
 ]
 d =c;

 #pragma rose [VarsExprsProductLattice:level=uninitialized \
 d:ConstantPropagationLattice:[level:constantValue,val=9] \
 ]
  a = d+1;
}
