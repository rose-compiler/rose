class Class_4 {};
class Class_3 {};
class Class_2 {};

template < typename t_parm_4 = Class_3,
           typename t_parm_5,
           typename t_parm_6 = Class_4 >
int Func_1(t_parm_5 && parm_2){
}

int var_1 = Func_1<Class_2>([&](){});
