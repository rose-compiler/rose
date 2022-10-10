int global = 1;

void func() {

}

void func2() {

}

void func_w_default(int& i1, void(*fp_w_default)() = func, void(*fp_w_default2)() = &func2, int& i2 = global) {

}

int global_2 = 2;

void func_2() {

}

void func2_2() {

}

// This function is not called. The address taken analysis should find the implicit address taking and reference creation anyway.
void func_w_default2(int& i1_2, void(*fp_w_default_2)() = func_2, void(*fp_w_default2_2)() = &func2_2, int& i2_2 = global_2) {

}

int main() {
  int i = 0;
  func_w_default(i);
}
