
class Test {
  public:
    Test() { 
      i = 24601;
      int* j;
      j = new int;
      // error, failed to delete j or assign another pointer to its address
      // before it went out of scope
    }
  int i;
  int m;
};

struct Test3 {
  int i;
  int x;
};
