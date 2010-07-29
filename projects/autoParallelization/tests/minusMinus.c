void foo(int numNodes, int numNodes2, int * x, int * nodelist)
{
  int j;

  for (j=numNodes-1 ; j>-1 ; --j) {
    if (x[j]<=0.) {
      numNodes2-- ;
      nodelist[j] = nodelist[numNodes2] ;
      nodelist[numNodes2] = j ;
    }
  }
}
