#include <initializer_list>

namespace std {
    typedef int string;

 // template<class _E>
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





#if 0
// gets unparsed to this

namespace std
{

typedef int string;

template < class _E >
    class initializer_list { };

template < typename _Tp >
    struct _Vector_base { };

template < typename _Tp >
    class vector
            : _Vector_base < _Tp > {
    public :
        typedef _Tp value_type;
        vector ( initializer_list < value_type > __l );
        int
        size ( );
     };

template < typename _Value >
    struct Struct_1 {
    public :
        void
        func_3 ( initializer_list < _Value > parm_3 );
    };
}

// NEEDED:
template < typename T >
int func_2 ( std :: vector < std :: string > & parm_2 ) {
    int i = parm_2 . size ( );
}

class Class_1 ;

class Class_1 
{
// NEEDED:
private: std::Struct_1< std::string  > member_1;
};

// NEEDED:

void func_1(std::vector< string  > &parm_1);
#endif

#if 0
which gets this error
rose_ROSE-68.cxx(39): error: identifier "string" is undefined
  void func_1(std::vector< string  > &parm_1);
#endif


              
