
template <int t_parm_3>
struct Class_1 {
    Class_1(Class_1<t_parm_3-1> parm_1) {}

    void func_1() {
      Class_1<0> * c;
    }
};
