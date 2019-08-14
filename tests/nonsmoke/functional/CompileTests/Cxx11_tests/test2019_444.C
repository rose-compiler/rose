#include <initializer_list>

namespace std {
    typedef int string;

    template<typename _Tp> struct _Vector_base { };

    template<typename _Tp>
    class vector
// NEEDED:
           : _Vector_base<_Tp> 
     {
    public:
        typedef _Tp value_type;
        vector(initializer_list<value_type> __l); // { }
     // vector(initializer_list<_Tp> __l) { }

        int size(); // { }
     };

    template<typename _Value>
    struct Struct_1 {
    public:
// NEEDED:
      int func_3(initializer_list<_Value> parm_3); // { }
    };
}

template < typename T >
int func_2(std::vector<std::string> &parm_2)  {
// NEEDED:
  int i = parm_2.size();
}

// Note: Class_1 forward declaration is generated at this locaton in the AST.

// Note: at this position in the AST is a compiler generated template instantiation for the class Struct_1 with a function declaration for funct_3.
// This causes "string" to be used as a template argument, and incorectly without name qualification.  I think this is related to why we loose the 
// name qualification in the non-compiler generated code below (where the bug is that "string" appears without name qualification.

class Class_1 {
// NEEDED:
    std::Struct_1<std::string> member_1;
};

// The problem is that the wrong SgTemplateArgument is used as the reference node to look up the qualificed name in the qualifiedNameMapForTypes.
// So we don't return the previously saved "std::" qualified name string.

// NEEDED:
// void func_1(std::vector<std::string>& parm_1);
void func_1(std::vector<std::string> parm_1);

