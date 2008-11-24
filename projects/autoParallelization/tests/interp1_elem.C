//#include <A++.h>
#include "simpleA++.h"

void interpolate1D(class floatArray &fineGrid,class floatArray &coarseGrid)
{
  int _var_0,i;
  int _var_1;
  int fineGridSize = fineGrid.length(0);
  int coarseGridSize = coarseGrid.length(0);
// Interior fine points
  class Range If(2,_var_1 = (fineGridSize - 2),2);
  class Range Ic(1,(coarseGridSize - 1),1);
#if 0
  for (_var_0 = 0; _var_0 <= -1 + (_var_1 + -1) / 2; _var_0 += 1) {
    fineGrid.elem(_var_0 * 2 + 2) = coarseGrid.elem(_var_0 * 1 + 1);
    fineGrid.elem(2 - 1 + _var_0 * 2) = (coarseGrid.elem(1 - 1 + _var_0 * 1) + coarseGrid.elem(_var_0 * 1 + 1)) / (2.0);
  }
#else  
  for (i = 1; i < _var_0; i += 1) {
   fineGrid.elem(i+1) =fineGrid.elem(i)+1;
  }
#endif  
}

