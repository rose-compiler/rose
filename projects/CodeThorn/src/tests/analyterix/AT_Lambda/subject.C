// TODO: lambda and l2 should not be in the address taken set because a "this" pointing to the lambda itself is not available.

int func() {
  return 1;
}

int main() {
  auto lambda = [](){};
  lambda();

  auto l2 = [](int& i_ref, bool b, int(*fp)()){
      &b;

    };

  int(*lp)() = []() -> int {
      return 1;
    };

  auto l3 = []() -> int {
    return 1;
  };

  auto l4 = []() -> int {
     return 1;
   };

  int i = 0;
  bool b_true = true;
  l2(i, b_true, func);
  l2(i, b_true, l3);

  int i2 = 2;
  int i3 = 3;
  int& ir3 = i3;
  auto l5 = [&]() -> int {
       i2 = 1;
     };

}

