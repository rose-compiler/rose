namespace namespace1 
   {
     template < int t_Parm1 >
     class t_Class1 
        {
          public:
               int func3() const {}
          private:
               t_Class1();
        };

     template < int t_Parm1 >
     t_Class1< t_Parm1 >::t_Class1() {}
   }

template < typename policy, typename kernel_type >
inline void t_func1( kernel_type kernel) {}

struct struct1 {};

int func2() 
   {
     namespace1::t_Class1< 0 > *var2;
     t_func1< struct1 >([=] { var2->func3(); } );
  // { var2->func3(); }
   }

 
#if 0
// gets unparsed to this:

namespace namespace1
{
template < int t_Parm1 >
class t_Class1 {
public :
   int func3 ( ) const;
private :
  t_Class1 ( );
};

template < int t_Parm1 >
t_Class1 < t_Parm1 > :: t_Class1 ( ) {
}
}

template < typename policy, typename kernel_type >
inline void t_func1 ( kernel_type kernel ) {
}

struct struct1
{
};

int func2()
{
namespace1::t_Class1< 0 > *var2;
::t_func1< struct1  > ( [=]
{
func3();
});
} 

// and results in this error:

rose_ROSE-27.cc(28): error: identifier "func3" is undefined
   func3();
   ^
#endif

 
