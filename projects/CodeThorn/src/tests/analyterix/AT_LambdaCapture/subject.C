
int main() {
  class A { };

  A  a1, a2, a3;
  auto l = [&a1, &a2, a3]() mutable {
    A* ap2 = &a2;
    A* ap3 = &a3;
  };

  A a4, a5;
  auto l2 = [&]() {
    A* ap5 = &a5;
    a4;
  };

  A a6, a7;
  auto l3 = [=, &a7]() mutable {
    A* ap6 = &a6;
  };

  class B {
    void mf() {
      A a8, a9;
      auto l4 = [=]() {
        const A* ap8 = &a8;
        const B* bp1 = this;
      };
      auto l5 = [&]() {
        const A* ap9 = &a9;
        const B* bp2 = this;
      };
      auto l6 = [&, this]() {
        const B* bp3 = this;
      };
      auto l7 = [this]() {
        const B* bp4 = this;
      };
    }
  };

  int i2 = 2;
  int i3 = 3;
  int& ir3 = i3;
  int i4 = 4;
  int i5;
  auto l5 = [&, i4]() -> int {
    i2 = 1;
    int* ip = &i2;
    const int* ip2 = &i4;
    int& ir4 = ir3;
    i5 = 2;
    return 1;
  };
}

