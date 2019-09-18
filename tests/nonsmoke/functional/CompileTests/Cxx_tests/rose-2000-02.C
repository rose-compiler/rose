typedef int size_t;

template <size_t t_parm_3>
struct Class_1 {
    void func_1(Class_1<0> v) {}
    void func_2(Class_1<t_parm_3-1> parm_1) {}
};
