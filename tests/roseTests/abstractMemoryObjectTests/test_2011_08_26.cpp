/*
 A test case from the original design doc
*/
class aggr {
  public:
    int i;
};

int foo() {
  aggr l;
  return l.i;
}
