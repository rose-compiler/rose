// ROSE-1989

// NOTE: not valid C++ code as in JIRA, but added #include <initializer_list> (below)
#include <initializer_list>

namespace std {
    typedef int string;

// NOTE: not valid C++ code as in JIRA, commented out class declaration for templated initializer_list (below)
//  template<class _E>
//  class initializer_list { };

    template<typename _Tp>
    struct _Vector_base { };

    template<typename _Tp>
    class vector
// NEEDED:
            : _Vector_base<_Tp> {
    public:
        typedef _Tp value_type;
        vector(initializer_list<value_type> __l) { }

        int
        size() { }
     };

    template<typename _Value>
    struct Struct_1 {
    public:
// NEEDED:
        void
        func_3(initializer_list<_Value> parm_3)
        { }
    };
}

template < typename T >
int func_2(std::vector<std::string> &parm_2)  {
// NEEDED:
    int i = parm_2.size();
}

class Class_1 {
// NEEDED:
    std::Struct_1<std::string> member_1;
};

// NEEDED:
void
func_1(std::vector<std::string>& parm_1);


// which gets:
// rose_ROSE-68.cxx(39): error: identifier "string" is undefined
//   void func_1(std::vector< string  > &parm_1);
//                            ^

