void func12(int) { }

void func13(int) { }

void func14(int) { }

void func15(int) { }

void func16(int) { }

void func17(int) { }

void func18(int) { }

void func19(int) { }

int main() {
  // ia = implicit address-of
  // ac = alias/ reference creation

  void(*func_ptr6)(int);
  func_ptr6 = /*ia*/func12;

  void(*func_ptr7)(int);
  func_ptr7 = /*ia*/*&*/*ia*/func13;

  void(&func_ref2)(int) = /*ac*/func14;
  void(&func_ref3)(int) = /*ac*/*/*ia*/*/*ia*/*&*/*ia*/*/*ia*/*&*/*ia*/*/*ia*/func15;
  void(*func_ptr5)(int) = /*ia*/*/*ia*/*/*ia*/*&*/*ia*/*/*ia*/*&*/*ia*/*/*ia*/func16;

  void(*func_ptr8)(int) = /*ia*/*/*ia*/func_ref2;

  void(*func_ptr9)(int) = /*ia*/*/*ia*/func17;

  void(*func_ptr10)(int) = &*/*ia*/func19;

  */*ia*/func18;

  void(**func_ptr11)(int) = 0;
  void(**func_ptr12)(int);
  func_ptr12 = func_ptr11;
}
