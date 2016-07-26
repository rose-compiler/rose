extern int ext;

extern int ext;

// Forward declaration 1:
int* getAddress(const int& var, ...);

// Forward declaration 2:
int* getAddress(const int& var, ...);

// Defining declaration:
int* getAddress(const int& var, ...) {
  return const_cast<int*>(&var);
}

int var;

namespace A {
  namespace {
    namespace B {
      int var1;
    }
  }
}

int func1(int a_arg, bool b_arg) {
  &var;
  &::A::B::var1;
  return b_arg ? a_arg + 1 : 2;
}

template<typename T>
class TestTemp {
 public:
  T t;
  // Get rid of address in mangled name before making this non-static.
  //  Address is added in SgTemplateArgument::get_mangled_name(...) in line
  //   mangled_type += StringUtility::numberToString(templateClassInstantiationDeclaration);
  static int mem_func_t(T t) {
    return 1;
  }
};

class Test2 {
 public:
  static int inner_test;
  int inner_test2;
} t2;

int* p123 = &t2.inner_test2;

class Test {
 public:
  static int im;
  int member;
  int bitfield: 2;

  class Test2 {
   public:
     static int inner_test;
     int inner_test2;
  } t2;
  
  Test() {
    member = 1;
    bitfield = 2;
    &t2.inner_test2;
  }
  
  void non_called_member_func() {  
    int i;
    i = 4;
  }
  
  void member_func() { }
   
  void member_func2(bool& b_ref, bool b2, int& i_ref, int i2) {
    &b2;
    member_func();
    class Test2 {
     public:
      int inner_test222;
    } t2;
    &t2.inner_test222;
    int inner_test223;
    & inner_test223;
    {
      int inner_test224;
      &inner_test224;
    }
  }
   
  static void static_mem_func() { }
  
  static void static_mem_func2();
   
  Test* getAddress() {
    return this;
  }
  
  ~Test() {
    im++;
  }
};

void Test::static_mem_func2() {
  static_mem_func();
}

int Test::im = 0;

void func2(void(*fp)(int)) {
  int s;
  &s;
 }

void (*getFuncAddress(void(&fr)(int)))(int) {
  return &fr;
}

void func(int i) {
  &ext;
}

void func_w_inner() {
  class Test2 {
    public:
      int inner_test2;
  } t2;

  &t2.inner_test2;
}

class c {
 public:
  void main() {
    int in_main_main;
    &in_main_main;        
  }
  int operator()() {
    int in_main_op;
    &in_main_op;
  }       
};

namespace a {
  namespace b {    
    int main() {
      {       
        int i2;
        &i2;
        int i3;
        &i3;
      }
    }
  }
}

void func_w_inner2() {
  class Test2 {
   public:
    int inner_test22;
  } t2;
  &t2.inner_test22;
  
  int inner_test23;
  &inner_test23;
}

template<typename A, typename B>
class TemplateClass {

};

void func_overloaded(TemplateClass<int, bool> t) {
  int i_ol;
  &i_ol;
}

void func_overloaded(Test t) {
  int i_ol;
  &i_ol;
}
void func_overloaded(int t) {
  int i_ol;
  &i_ol;
}

int& getRef(int& ik) {
  return ik;
}

int global = 0;
void func3(int& m) {
  global = 1; // Changes value of m because func3 is called with global as argument below.
}

int main() {
  func3(global);

  {
    class Test2 {
     public:
      class Test3 {

      };
      int r2;
      void mem_func() {
        int r;
        &r;
        &r2;
      }
      int inner_test5;
    } t222;
    &t222.inner_test5;

    class Test4 {
     public:
       static void mf(Test2::Test3 a) {}
       static void mf(Test2 a) {}
       static void mf(::Test2 a) {}
    } tasdasd;

    void(*fppp)(Test2::Test3) = &Test4::mf;
    void(*fppp2)(Test2) = &Test4::mf;
    void(*fppp3)(::Test2) = &Test4::mf;
  }

  {
    class Test2 {
     public:
       static void mf(Test2 a) {}
       static void mf(::Test2 a) {}
    } tasdasd;

    void(*fppp2)(Test2) = &Test2::mf;
    void(*fppp3)(::Test2) = &Test2::mf;

    for(int iter = 0; iter != 30; ++iter) {
      iter++;
    }

    int i = 1, i2 = 2, i3 = 3;
    int* ip = getAddress(i, i2, i3);
    *ip = 2;
    ip[0] = 3;
    Test t;
    Test t2;
    Test t3;
    Test t4;
    Test t6;
    Test t7;

    int* member_pointer = &(t6.member);

    int k;
    int k2 = 1;
    int k3 = 1;
    int& k4 = getRef(k2);

    int& kr3 = k4;
    int& kr2 = (i ? k2 : k3);

    int k5 = k;

    void(Test::*test_mem_func_ptr)(bool&, bool, int&, int) = &Test::member_func2;
    void(Test::*test_mem_func_ptr2)() = &Test::member_func;
    void(*test_static_mem_func_ptr)() = &Test::static_mem_func;

    bool b1 = true;
    bool b2 = false;
    bool b3 = false;
    int i4 = 3;
    int i5 = 5;

    (i3 == 4 ? t : t2).member_func2(b1, false, i4, 6);
    ((((i2 == 4 ? t3 : t4).*test_mem_func_ptr)))(b2, false, i5, 6);
    ((((i2 == 4 ? t3 : t4).*test_mem_func_ptr)))(b2, b3, i5, 6);

    void(*func_ptr_overload)(Test) = &func_overloaded;
    void(*func_ptr_overload2)(int) = &func_overloaded;
    void(*func_ptr_overload3)(TemplateClass<int, bool>) = &func_overloaded;

    void(*func_ptr)(int) = func;
    void(*&ref_func_ptr)(int) = func_ptr;

    func2(func);
    func2(&func);
    getFuncAddress(func);

    func_ptr(2);
    ref_func_ptr(2);
    getFuncAddress(func)(2);

    Test t5;
    Test* t5p = t5.getAddress();

    TestTemp<Test> t_temp;
    Test t8;
    t_temp.mem_func_t(t8);

    Test t9;
    Test::static_mem_func();
    t9.static_mem_func();

    Test t9_2;
    t9_2.static_mem_func2();

    Test::static_mem_func2();
  }
  int* k = &Test::im;
  &::Test::Test2::inner_test;
  &::Test2::inner_test;
}
