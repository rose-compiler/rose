class ostream 
   {
   };

// template <typename T> ostream& operator<< <>(ostream  & a_os,const int a_iv);
// template <typename T> ostream& foo (ostream  & a_os,const int a_iv);

template<typename T> class IndexTM;

// template <typename T> ostream& operator<< (ostream  & a_os,const int a_iv);
template <typename T> ostream& operator<< (ostream  & a_os,const IndexTM<T> & a_iv);

template<typename T> 
class IndexTM
   {
     public:
       // friend std::ostream& operator<< <>(std::ostream  & a_os,const IndexTM & a_iv);
          friend ostream& operator<< <>(ostream  & a_os,const IndexTM<T> & a_iv);
#if 0
          template <typename T> friend ostream& operator<< <T>(ostream & a_os, const IndexTM & a_iv);
#endif
#if 0
      template <> ostream& operator<< <T>(ostream  & a_os,const int & a_iv);
#endif
#if 0
   // template <> ostream& foo(ostream  & a_os,const int a_iv);
      template <typename S> ostream& foo(ostream  & a_os,const int a_iv);

      template <typename S> friend ostream& operator<< (ostream  & a_os,const int & a_iv);

      template <typename S> friend ostream& operator<< (ostream & a_os, const IndexTM & a_iv);
#endif
   };

