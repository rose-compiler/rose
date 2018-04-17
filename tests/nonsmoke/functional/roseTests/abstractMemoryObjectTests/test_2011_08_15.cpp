/*
 A test case from the original design doc
*/
class aggr;
class aggr {
  public:
    int i;
    double d;
    aggr* pAggr;
};

int foo() {
  int a, b, c;  
  int *pa = &a, *pb = &b;
  int x[100];
  aggr l;
  l.pAggr = new aggr();
  return a + *pb + x[5] + l.pAggr->i + l.d;
}
