#if 0
#include<map>

std::map<int,int> xxx;
#endif


#if 1
// Simpler problem that demonstrates case of: decl->get_symbol_from_symbol_table() != NULL
// WARNING: set_function_modifier is incomplete ...
struct Y
   {
     template<typename T> void foo();
     template<typename T> void foo() const;
   };
#endif





#if 0
// Simpler code demonstrating the same bug as the map<> example demonstrates.
template<typename T> void foo();
template<typename T> void foo();

template<typename T> void foo(int);
template<typename T> void foo(long);

template<typename S> 
class X
   {
     public:
          template<typename T> void foo(int x);
          template<typename T> void foo(long x);
   };
#endif



#if 0
// template<typename T> void X::foo();
template<typename S> 
template<typename T> void X<S>::foo(int x){}
#endif

#if 0
// This works fine.
void foo();
void foo();
#endif
