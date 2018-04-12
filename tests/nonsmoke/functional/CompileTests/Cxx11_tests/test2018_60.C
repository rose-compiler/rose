
template< class _T1, class _T2> struct t_struct5 {};

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

template< typename t_t_parm4, typename t_t_parm3>
class t_Class2 
   {
     public:
          typedef t_struct1<t_t_parm4> typedef1;

          template< typename t_t_parm2> t_struct5<typedef1, bool> t_method1();
   };

template<typename t_t_parm6> class t_Class3 {};

template <typename t_t_parm1 >
class t_Class1 
   {
     public:
          typedef t_Class3< const t_t_parm1 > typedef2;

     private:
          typedef typename t_struct3< t_Class4< typedef2 > >::template t_struct2< typedef2 >::typedef7 typedef4;

     public:
          typedef typename t_Class2<typedef2,typedef4 >::typedef1 typedef6;
   };

// BUG: A private type is referenced.
// The original code appears as:
// t_struct5< t_Class1< char >::typedef6, bool > local2;
// and the generated code appears as:
// t_struct5< t_Class2< t_Class1< char > ::typedef2 ,t_Class1< char > ::typedef4 > ::typedef1  , bool  > local2;
t_struct5< t_Class1< char >::typedef6, bool > local2;


#if 0
    Produces this error:

    rose_ROSE-1142_reproducer.cc:59:5: error: "typedef t_struct3<t_Class4<t_Class3<const char> > >::t_struct2<t_Class3<const char> >::typedef7 t_Class1<char>::typedef4" is private

         typedef4;

         ^

    rose_ROSE-1142_reproducer.cc:66:1: error: within this context

     t_struct5< t_Class2< t_Class1< char > ::typedef2 ,t_Class1< char > ::typedef4 > ::typedef1  , bool  > local2;

     ^

#endif
 

 
