// Here is the reproducer and error for https://rzlc.llnl.gov/jira/browse/ROSE-59 and https://rosecompiler.atlassian.net/browse/ROSE-1713:
// reproducer (has two similar cases which are probably same error) code:

template<typename Type_4,
        typename ... Types_3>
struct Struct_1 {
};

class Class_2
{
}
;

class Class_2 global_3;

template<typename Type_3>
class Class_5 {
public:
    template<typename Type_5>
    Class_5(Type_5 &&parm_2);
    Type_3 &operator*() const;
};

namespace Class_4_2_Parm
{
    template<typename Type_6,
        typename ... Types_2>
    class Class_4 {
    public:
        using typedef_1 =
        Struct_1<int,
                Types_2 ...>;
    };
    template<typename ... Types_1>
    typename Class_4<Types_1 ...>::typedef_1 func_1(Class_4<Types_1 ...> parm_1) { }
    class Class_5<Class_4<int, double> > global_2(global_3);
    Class_4<int, double>::typedef_1 global_1 = func_1((*global_2));
}

namespace Class_4_1_Parm
{
    template<typename ... Types_2>
    class Class_4 {
    public:
        using typedef_1 =
        Struct_1<int,
                Types_2 ...>;
    };
    template<typename ... Types_1>
    typename Class_4<Types_1 ...>::typedef_1 func_1(Class_4<Types_1 ...> parm_1) { }
    class Class_5<Class_4<int> > global_2(global_3);
    Class_4<int>::typedef_1 global_1 = func_1(( * global_2));
}

 
#if 0
gets this error:
rose_ROSE-59.cpp(31): error: no suitable user-defined conversion from "Struct_1<int>" to "Struct_1<int, double>" exists

  Class_4< int ,double > ::typedef_1 global_1 = func_1(( * global_2));

                                                ^

 

rose_ROSE-59.cpp(45): error: expected a type specifier

  Class_4< ,int > ::typedef_1 global_1 = func_1(( * global_2));

           ^

 

rose_ROSE-59.cpp(45): error: no suitable user-defined conversion from "Struct_1<int, int>" to "Struct_1<int, <error-type>, int>" exists

  Class_4< ,int > ::typedef_1 global_1 = func_1(( * global_2));

                                         ^
#endif
 
#if 0
lines 30 and 45 in the reproducer:

{code}

    class Class_5<Class_4<int, double> > global_2(global_3);

...

    Class_4<int>::typedef_1 global_1 = func_1(( * global_2));

{code}

get unparsed to:

{code}

class Class_5< Class_4< int  >  > global_2(global_3);

...

Class_4< ,int > ::typedef_1 global_1 = func_1(( * global_2));

{code}
#endif

 
