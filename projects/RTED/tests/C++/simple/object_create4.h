class Test3;

class Test {
  public:
    Test() { 
    }
  int i;
 private:
  int m;
  Test3* t;
};

class Test3 {
 public:
  Test3();
  ~Test3(){};
  int i;
  int x;
};
