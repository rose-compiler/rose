// This test shows how our SDG based IFC may fail due to not implementing
// Horwitz's linkage grammer.
// Since tainted data is passed into add, and passed back out to badres
// without an linkage grammer, it is not clear if res may get that 
// bad data.

int add(int a, int b) {
  return a + b;
}

int main() {

  int bad;
  int good;
  int badres;
  int res;

#pragma leek tainted
  bad  = 0;
  good = 1;

  badres = add(bad, 1);
  res = add(good, 1);

#pragma leek trusted
  return res;

}
