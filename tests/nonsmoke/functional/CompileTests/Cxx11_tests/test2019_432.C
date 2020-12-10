
namespace std 
   {
     template<typename... _Elements> class tuple { };

     template<> class tuple<> { };
   }

class _Rb_tree 
   {
     public:
          template<typename... _Args> void _M_emplace_hint_unique(_Args &&... __args) { }
   };

_Rb_tree _M_t;

void func_3() 
   {
     _M_t._M_emplace_hint_unique( std::tuple<>() );
   }

