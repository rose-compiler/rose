typedef __SIZE_TYPE__ size_t;

template <size_t t_parm_3>
class Class_1 {
public:
    Class_1()
    { }

    Class_1(Class_1<t_parm_3-1> parm_1)
    { }

    Class_1<0> func_1() {
        return Class_1<0>();
    }
};
