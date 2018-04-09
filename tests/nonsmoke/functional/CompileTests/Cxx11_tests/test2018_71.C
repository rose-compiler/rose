
template< class _T1> struct t_struct5 {};

template< typename t_t_parm10>
class t_Class4 
   {
     public:
          template< typename t_t_parm9>
          struct t_struct4
             {
               typedef t_Class4< t_t_parm9 > typedef8;
             };
   };

template< typename t_t_parm7>
struct t_struct3 
   {
     template< typename t_t_parm8>
     struct t_struct2 
        {
          typedef typename t_t_parm7::template t_struct4< t_t_parm8 >::typedef8 typedef7;
        };
   };

template< typename t_t_parm5 > struct t_struct1 {};

template< typename t_t_parm4 >
class t_Class2 
   {
     public:
          typedef t_t_parm4 typedef1;

          t_struct5<typedef1> t_data1;
   };

template<typename t_t_parm6> class t_Class3 {};

template <typename t_t_parm1 >
class t_Class1 
   {
     public:
          typedef t_Class3< const t_t_parm1 > typedef2;

  // private:
     protected:
          typedef typename t_struct3< t_Class4< int > >::t_struct2< t_t_parm1 >::typedef7 typedef4;

     public:
          typedef typename t_Class2<typedef4>::typedef1 typedef6;
   };

t_struct5< t_Class1< char >::typedef6 > local2;

