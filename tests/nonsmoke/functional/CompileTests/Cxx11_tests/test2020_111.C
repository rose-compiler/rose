template<typename _Tp>
class allocator
   {
     public:
           template<typename _Tp1>
           struct rebind
           { typedef allocator<_Tp1> other; };

           allocator() { }
   };

struct _List_node_base
   {
   };

template<typename _Tp>
class _List_base
   {
     typedef typename allocator<_Tp>::template rebind<_List_node_base >::other _Node_alloc_type;
   };

namespace Namespace_And_Class_1 
   {
     class Namespace_And_Class_1 {};
     enum Type_1 
        {
          ENUM_VALUE_1 = 0,
        };
     void func_1(Type_1 parm_1);
     class Class_1 
        {
          virtual void func_2();
          struct Struct_1 
             {
               Struct_1();
             };
        };
   }

using namespace Namespace_And_Class_1;

Class_1::Struct_1::Struct_1()
   {
//NEEDED:  
     _List_base<int> var_1;
   }

void Class_1::func_2()
   {
     func_1(ENUM_VALUE_1);
   }

