void func() {
  int i=0,z=0;
  int t = 3;    //Line 1
  // increments t t=i and then (t++)
  (t = i)++;  //Line 2
  z = t;         //Line 3
  z++;
}

