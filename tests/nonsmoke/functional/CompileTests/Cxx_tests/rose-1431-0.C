namespace Namespace1 {

  class Class2 {};

  template <typename T_T_Parm1>
  class T_Class1 {
    public:
      int func2() const;

    protected:
      void func1();
  };

  template <typename T_T_Parm2>
  void T_Class1<T_T_Parm2>::func1() {
    int local1 = func2();
  }

}

template class Namespace1::T_Class1<Namespace1::Class2>;
