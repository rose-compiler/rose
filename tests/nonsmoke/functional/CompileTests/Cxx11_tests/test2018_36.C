template<class T> 
class t_Class0 {};

namespace Namespace1 {}
// Separate namespaces needed above and below!

namespace Namespace1
{
template <typename t_ty_Parm2>
class t_Class4 {
   public:
   typedef int Typedef4; 
}; // end t_Class4

template <class t_cl_Parm9>
class t_Class6 {};

template <typename t_ty_Parm11, 
          typename t_ty_Parm12>
struct t_Struct1 {};

template <typename t_ty_Parm13>
struct t_Struct1 <t_ty_Parm13, 
                  typename t_ty_Parm13::Typedef4>
{
   typedef int t_ty_Typedef7;
}; // end t_Struct1

template<typename t_ty_Parm14, 
         typename t_ty_Parm15>
   t_Class0<t_Class6<typename t_Struct1<t_ty_Parm14, 
                     t_ty_Parm15>::t_ty_Typedef7> >
   t_func1();

template<> 
   t_Class0<t_Class6<t_Struct1<t_Class4< int >, 
                     t_Class4< int >::Typedef4>::t_ty_Typedef7> >
   t_func1<t_Class4< int >, 
           t_Class4< int >::Typedef4>() 
{
   t_Class0<t_Class6<t_Struct1<t_Class4< int >, 
            t_Class4< int >::Typedef4>::t_ty_Typedef7> > var1;
   return (var1);
} // end t_func1
} // end Namespace1

