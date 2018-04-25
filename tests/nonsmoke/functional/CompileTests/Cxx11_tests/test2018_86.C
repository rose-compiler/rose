
namespace namespace1 {
   template<typename t_t_parm3>
   class vector {};
}

template <typename t_t_parm1>
class t_Class1 
   {
     public:
          template <typename t_t_parm2>
          namespace1::vector <int> t_func1() {}
   };

template namespace1::vector <int> t_Class1 <int>::t_func1 <int>();

 
