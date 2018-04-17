
void func10() { }

class Test2 {
 public:

};

class Test {
 public:
  void (*fp)();

  
  Test(void (&func_arg)())
    : fp(/*implicit address-of*/func_arg) {

  }

  Test()
    : fp(/*implicit address-of*/func10) {

  }

  void non_called_member_func() {  
    int i;
    i = 4;
  }
  
  void member_func() { }
   
  void member_func2(bool& b_ref, bool b2, int& i_ref, int i2) {

  }
   
  static void static_mem_func() { }
  
  static void static_mem_func2();
   
};

void Test::static_mem_func2() {
  static_mem_func();
}

void func2(void(*fp)(int)) {

 }

void (*getFuncAddress(void(&fr)(int)))(int) {
  void (*func_ptr_in_func)(int) = /*implicit address-of*/fr;
  return func_ptr_in_func;
}

void (*getFuncAddress2(void(&fr2)(int)))(int) {
  return /*implicit address-of*/fr2;
}

void func_as_default_arg() { }

void func_w_default(void(*fp_w_default)() = /*implicit address-of*/func_as_default_arg) { }

void func(int i) { }

void func3(int i) { }

void func4(int i) { }

void func5(int i) { }

void func6(int i) { }

void func7(int i) { }

void (*getFuncAddress())(int) {
  return /*implicit address-of*/func7;
}

template<typename A, typename B>
class TemplateClass { };

void func_overloaded(TemplateClass<int, bool> t) { }

void func_overloaded(Test t) { }

void func_overloaded(int t) { }

void func_overloaded(bool t) { }

void func8(int i) { }

void func11(int i) { }

void func12(int i) { }

void func13(int i) { }

void func14(int i) { }

void func15(int i) { }

void func16(int i) { }

void func17(int i) { }

void func18(int i) { }

int main() {


  class Test2 {
   public:
     static void mf(Test2 a) {}
     static void mf(::Test2 a) {}
  } tasdasd;

  void(*fppp2)(Test2) = /*implicit address-of*/Test2::mf;
  void(*fppp3)(::Test2) = /*implicit address-of*/Test2::mf;
  
  void(Test::*test_mem_func_ptr)(bool&, bool, int&, int) = &Test::member_func2; // implicit address-of of non-static member function not possible
  void(Test::*test_mem_func_ptr2)() = &Test::member_func; // implicit address-of of non-static member function not possible

  void(*test_static_mem_func_ptr)() = /*implicit address-of*/Test::static_mem_func;

  void(*func_ptr_overload)(Test) = /*implicit address-of*/func_overloaded;
  void(*func_ptr_overload1)(bool);
  func_ptr_overload1 = /*implicit address-of*/func_overloaded;
  void(*func_ptr_overload2)(int) = /*implicit address-of*/func_overloaded;
  void(*func_ptr_overload3)(TemplateClass<int, bool>) = /*implicit address-of*/func_overloaded;

  void(*func_ptr)(int) = /*implicit address-of*/func;
  void(*&ref_func_ptr)(int) = func_ptr;
  ref_func_ptr = /*implicit address-of*/func13;

  void(&func_ref)(int) = func5;
  void(*func_ptr2)(int) = /*implicit address-of*/func_ref;

  void(*func_ptr3)(int) = &func11;
  void(*func_ptr4)(int) = &*func_ptr3; // No address taking
  void(*func_ptr5)(int);
  func_ptr5 = func_ptr4; // No address taking

  void(*func_ptr6)(int);
  func_ptr6 = /*implicit address-of*/func12;

  static_cast<void(*)(int)>(/*implicit address-of*/func14);
  static_cast<void(*)(int)>(&func15);
  reinterpret_cast<void(*)(int)>(/*implicit address-of*/func16);
  (void(*)(int))(/*implicit address-of*/func17);

  func2(/*implicit address-of*/func6);
  getFuncAddress(func3);

  func_ptr(2);
  ref_func_ptr(2);
  getFuncAddress(func4)(2);

  try {
    throw /*implicit address-of*/func18;
  }
  catch(void(*fp)(int)) {

  }
}

