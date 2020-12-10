// ROSE-1786

namespace namespace_1 {
    typedef int typedef_2;
    template<typename _Tp>
    struct struct_1 {
    };

    template<typename tt_parm_3,
            typename tt_parm_2,
            typename tt_parm_1 = namespace_1::struct_1<tt_parm_3> >
    class Class_2 {
    };
}
class Class_1 {
    using typedef_1 = namespace_1::Class_2<namespace_1::typedef_2, int>;
};

// produces error:
// rose_ROSE-60.cpp(17): error: identifier "typedef_2" is undefined
//   using typedef_1 = class ::namespace_1::Class_2 < typedef_2 , int , struct_1< typedef_2 > > ;
//                                                    ^
// 
// rose_ROSE-60.cpp(17): error: struct_1 is not a template
//   using typedef_1 = class ::namespace_1::Class_2 < typedef_2 , int , struct_1< typedef_2 > > ;
//                                                                      ^
// 
// rose_ROSE-60.cpp(17): error: identifier "typedef_2" is undefined
//   using typedef_1 = class ::namespace_1::Class_2 < typedef_2 , int , struct_1< typedef_2 > > ;
// 

