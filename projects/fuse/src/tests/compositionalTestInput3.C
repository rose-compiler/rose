#pragma fuse lc(ccs, ccs, ld, cp, dp, cp, dp, cp, oa, cp, dp, oa, cp)

void CompDebugAssert(bool);

int min(int a, int b)
{
  if(a > b) {
    return b;
  } else {
    return a;
  }
}

int max(int a, int b)
{
  if(a < b) {
    return b;
  } else {
    return a;
  }
}

int main()
{
  int w=15;
  int x=10;
  int y=20;
  CompDebugAssert(w==15);
  CompDebugAssert(x==10);
  CompDebugAssert(y==20);
  int z = max(x, y);
  int array[1000];

  // ConstProp: w=15, x=10, y=20, z=20
  CompDebugAssert(w==15);
  CompDebugAssert(x==10);
  CompDebugAssert(y==20);
  CompDebugAssert(z==20);
  array[z] = min(w,z);
        
  // ConstProp: w=15, x=10, y=20, z=20, array[20]==15
  CompDebugAssert(w==15);
  CompDebugAssert(x==10);
  CompDebugAssert(y==20);
  CompDebugAssert(z==20);
  CompDebugAssert(array[20]==15);
  if(array[y]==15) {
    // ConstProp: w=15, x=10, y=20, z=15, array[20]==15
    CompDebugAssert(w==15);
    CompDebugAssert(x==10);
    CompDebugAssert(y==20);
    CompDebugAssert(z==20);
    CompDebugAssert(array[y]==15);
    array[array[y]]=123;
  } else {
    // ConstProp: w=15, x=10, y=20, z=15, array[20]!=15
    CompDebugAssert(w==15);
    CompDebugAssert(x==10);
    CompDebugAssert(y==20);
    CompDebugAssert(z==20);
    //CompDebugAssert(array[y]!=15);
    array[z]=456;
  }
  // ConstProp: w=15, x=10, y=20, z=15, array[20]==15, array[15]=123
  CompDebugAssert(w==15);
  CompDebugAssert(x==10);
  CompDebugAssert(y==20);
  CompDebugAssert(z==20);
  /*CompDebugAssert(array[20]==15);*/
  CompDebugAssert(array[15]==123);

  return z+array[y]+array[z];
}
                
