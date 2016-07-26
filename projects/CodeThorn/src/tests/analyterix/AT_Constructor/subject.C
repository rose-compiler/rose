class Test {
public:
  Test(int& arg) {
    &arg;
  }
};

class Test2 {
  int val2;
};

class Test3 {
 public:
  int val;
  Test3() : val(0) {

  }
  // Copy constructor:
  Test3(const Test3& other) : val(other.val) {

  }
};

int main() {
  int i = 0;
  Test t(i);
  int i2 = 0;
  Test* tp = new Test(i2);

  Test2 t2;
  Test2 t3 = t2;

  Test2 t4;
  Test2 t5(t4);

  Test2 t6, t7;
  t6 = t7;

  Test3 t8;
  Test3 t9(t8);
}
