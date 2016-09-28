// bug 355 reported by Sri Hari Krishna Narayanan @ ANL
#include<assert.h>
#include<stdio.h>
int main() {
  double grid[3] [4] = { [0][1]=8, [2][1]=6};
  int i, j, sum=0;
  for (i=0; i<3; i++) 
     for (j=0; j<4; j++)
       sum+=grid[i][j];
  assert(sum==14);
  return 0;
} 
