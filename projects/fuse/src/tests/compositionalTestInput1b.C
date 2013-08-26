#pragma fuse lc(ccs, ld, oa, cp)
//, oa, cp, oa, cp, oa, cp)
void CompDebugAssert(bool);
int global;

int noProp(int q) {
  q = 101;
  return 102;
}
int PropThroughArg(int& q) {
  q += 201;
  int dummyToTestForNoProp;
  return 202;
}
int PropThroughRet1(int q) {
  q += 301;
  return q;
}
int PropThroughRet2(int q) {
  q += 401;
  return q;
}
int PropThroughRet3(int q) {
  q += 501;
  return q;
}
int PropThroughRet4(int q) {
  q += 601;
  return q;
}

int main()
{
  int a=1;
  int b=2;
  int c=3;
  int d=4;

/*  CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);*/
  // ConstProp: a=1, b=2, c=3, d=4
  // LiveDead: a, b, c, d
  int W = noProp(a);
  while(a) 
    W = noProp(a);
  
/*  CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(W==102);*/
  // ConstProp: a=1, b=2, c=3, d=4, W=102
  // LiveDead: a, b, c, d, W
  int X;
  if(a)
    X = PropThroughArg(W);
  else {
    X = 202;
    W = 303;
  }
  
  CompDebugAssert(a==1);
  CompDebugAssert(b==2);
  return b+a+X+W;
  /*CompDebugAssert(c==3);
    CompDebugAssert(X==202);
    CompDebugAssert(W==303);
    // ConstProp: a=1, b=2, c=3, d=4, X=202, W=303
    // LiveDead: a, b, c, d, X, W
    int Y = PropThroughRet1(X)+W;
  
    CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(Y==806);
    // ConstProp: a=1, b=2, c=3, d=4, Y=806
    // LiveDead: a, b, c, d, Y
    int array[100000];
    int array2D[10000][10000];

    CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(Y==806);
    // ConstProp: a=1, b=2, c=3, d=4, Y=806
    // LiveDead: a, b, c, d, Y
    array[a] = PropThroughRet3(Y);

    CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(Y==806);
    CompDebugAssert(array[1]==1307);
    // ConstProp: a=1, b=2, c=3, d=4, Y=806, array[1]=1307
    // LiveDead: a, b, c, d, Y, array[1]
    array[b] = Y;
  
    CompDebugAssert(a==1);
    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(array[1]==1307);
    CompDebugAssert(array[2]==806);
    // ConstProp: a=1, b=2, c=3, d=4, array[1]=1307, array[2]=806
    // LiveDead: a, b, c, d, array[1], array[2]
    array[array[d-c]+array[b]] = array[a];

    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(array[2]==806);
    CompDebugAssert(array[2113]==1307);
    CompDebugAssert(array[2113]==array[array[d/d]+array[d-b]]);
    // ConstProp: b=2, c=3, array[2113]=1307, array[2]=806
    // LiveDead: b, c, array[2], array[2113]
    array2D[3][array[2113]+401+601] = array[b];

    CompDebugAssert(b==2);
    CompDebugAssert(c==3);
    CompDebugAssert(array[2113]==1307);
    CompDebugAssert(array[2113]==array[array[d/d]+array[d-b]]);
    CompDebugAssert(array2D[3][2309]==806);
    // ConstProp: c=3, array[2113]=1307, array2D[3][2309]=806
    // LiveDead: c, array[2113], array2D[3][2309]
    array[c] = PropThroughRet2(PropThroughRet4(array[2113]));
  
    CompDebugAssert(array[c]==2309);
    CompDebugAssert(array[3]==array[c]);
    CompDebugAssert(array2D[3][2309]==806);
    // ConstProp: array[3]=2309, array2D[3][2309]==806)
    // LiveDead: array[3], array2D[3][2309]

    return array[3]+array2D[d-a][array[c]];*/
}
