// vector constructors
// semantics
// implementation: 
#include <vector>
#include <iostream>

using namespace std;

int main (void)
{
  int i,j= 268435456;
  int y;
  //int j= 500000000;
  vector <float> v1(j,56.00);  //stmt 2.

  for (i=0;i<v1.size();i++ )
  {
    v1[i]=v1[i]+1;
    y= v1[v1.size()-1];
  }
  return 0;
}
