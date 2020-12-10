template<typename t_parm_2>
struct struct_1
{
    static void func_1();
};

template<typename t_parm_3>
class Class_1
{
    friend
    struct struct_1<Class_1>;
    void func_2();
};

template<typename t_parm_1>
void Class_1<t_parm_1>::func_2()
{
    struct_1<Class_1>::func_1();
}
