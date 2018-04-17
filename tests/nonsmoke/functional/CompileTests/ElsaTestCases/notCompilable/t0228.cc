// t0228.cc
// test a recent fix to 'computeArraySizeFromLiteral'

void f()
{
  char sName[] = "SOAPPropertyBag";
  char arr[16];

  // should be same type
  __checkType(sName, arr);

  // not the same type!
  char const arr2[16];
  //ERROR(1): __checkType(sName, arr2);
  
  // this is not a legal type
  // UPDATE: dsw: I moved this test to t0228b.cc so I can turn it off
  // in the presence of GNU
  //ERROR - OFF - (2): char arr3[];
}
