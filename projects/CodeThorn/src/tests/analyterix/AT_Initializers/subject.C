void func() { }

void func2() { }

void func3() { }

void func4() { }

void func5() { }

void func6() { }

void func7() { }

class Test {
public:
  int& m_i;
  void (*m_fp)();
  bool m_b;
};

int gi = 1;
bool gb = true;

class Test2 {
  Test t;
public:
  Test2() : t{gi, func7, gb} {

  }
};

int gi2 = 2;
bool gb2 = false;
void func8() { }

void func9(Test t = {gi2, func8, gb2}) {

}
int main() {
  int i = 1;
  bool b = false;
  // Aggregate initialization:
  //Test t2 = {i, func, b}; // ROSE assertion (TODO)

  int i2 = 2;
  bool b2 = true;
  Test t2 = {i2, func2, b2};

  int i3 = 3;
  bool b3 = false;
  Test t3 = {i3, func3, b3};

  int i4 = 4;
  bool b4 = true;
  Test* tp = new Test{i4, func4, b4};

  int i5 = 5;
  bool b5 = false;
  // Compound literal initialization (but appears as aggregate initialization in the ROSE AST):
  Test t5 = (Test){i5, func5, b5};

  int i6 = 6;
  bool b6 = false;
  // Compound literal (SgCompoundLiteralExp) (node without children in the ROSE AST): not supported
  //(Test){i6, func6, b6};
}
