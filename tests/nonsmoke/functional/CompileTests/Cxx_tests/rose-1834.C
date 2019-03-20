template<int t_parm_1>
struct Class_1 {
  Class_1() { }

  Class_1<t_parm_1 + 1> operator[](const int &parm_2) {}

  static Class_1<0> method_1() {
    return Class_1<0>();
  }
};
