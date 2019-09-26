// ROSE-2097
struct struct_1;

template<
        typename t_t_parm_3,
        typename t_t_parm_4>
class Class_1;

template< 
        typename t_t_parm_5,
        typename t_t_parm_6>
struct struct_and_class_4;

template<
        typename t_t_parm_8,
        typename t_t_parm_9>
struct struct_and_class_4<
        Class_1<t_t_parm_8,
                t_t_parm_9>,
        struct_1>

{
public:
    class Class_2;
};

template<
        typename t_t_parm_10,
        typename t_t_parm_11>
class struct_and_class_4<
        Class_1<t_t_parm_10,
                t_t_parm_11>,
        struct_1>::Class_2
        : public struct_and_class_4<
                Class_1<t_t_parm_10,
                        t_t_parm_11>,
                struct_1>::NoSuchClass
{
};
