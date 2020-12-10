#if 1
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
#endif

#if 1
template<typename _Tp>
class _List_base
   {
     typedef typename allocator<_Tp>::template rebind<_List_node_base >::other _Node_alloc_type;
  // typedef int _Node_alloc_type;
   };
#endif

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
#if 1
          struct Struct_1 
             {
               Struct_1();
             };
#endif
        };
   }

using namespace Namespace_And_Class_1;

#if 1
Class_1::Struct_1::Struct_1()
   {
//NEEDED:  
     _List_base<int> var_1;
   }
#endif

void Class_1::func_2()
   {
  // Original code:                     func_1(ENUM_VALUE_1);
  // Unparse as: Namespace_And_Class_1::func_1(ENUM_VALUE_1);
     func_1(ENUM_VALUE_1);
   }

