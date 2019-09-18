#define FOO

#ifdef FOO
template <typename t_parm_1>
class Class_1
{
    void func_1() { }
};
#endif

// unparses to this:
// #define FOO
// #ifdef FOO
// template < typename t_parm_1 >
// class Class_1
// {
//     void func_1 ( );
// };

// which gets this error:
// rose_ROSE-69.cxx(2): error: the #endif for this directive is missing
//   #ifdef FOO
//    ^



