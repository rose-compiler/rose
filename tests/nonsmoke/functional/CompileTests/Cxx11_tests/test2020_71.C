// ROSE-1989


#include <initializer_list>

namespace std {

    typedef int string;

 

    template<class _E>

 // DQ (8/22/2020): It is required to use the initializer_list header file.
 // class initializer_list { };
 

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

 
