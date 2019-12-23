template<typename t_t_parm_15>

class Class_6 {

public:

    static void

    func_5() {}

//NEEDED:

    template<typename t_t_parm_16, typename t_t_parm_17>

    void

    func_6(t_t_parm_17 t_t_parm_16::* const& parm_6) {}

};

 

template<typename t_t_parm_13, typename t_t_parm_14>

class Class_5;

 

template<typename t_t_parm_10, typename t_t_parm_11, typename... t_t_parm_12>

class Class_5<t_t_parm_10(t_t_parm_12...), t_t_parm_11>

        : public Class_6<t_t_parm_11> {

};

 

template<typename t_t_parm_3>

class Class_4;

 

template<typename t_t_parm_7, typename... t_t_parm_8>

class Class_4<t_t_parm_7(t_t_parm_8...)> {

    typedef t_t_parm_7 typedef_1(t_t_parm_8...);

public:

    template<typename t_t_parm_9, typename = void>

    Class_4(t_t_parm_9);

};

 

template<typename t_t_parm_4, typename... t_t_parm_5>

template<typename t_t_parm_6, typename>

Class_4<t_t_parm_4(t_t_parm_5...)>::Class_4(t_t_parm_6 parm_5) {

//NEEDED:

    Class_5<typedef_1, t_t_parm_6>::func_5();

}


void func_3() {
    Class_4<void()> func_1 = [&]() { };
}
