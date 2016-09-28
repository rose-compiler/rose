template<typename T> class IndexTM;

template <typename T> int foo (const IndexTM<T> & a_iv);

template<typename T> 
class IndexTM
   {
     public:
          friend int foo <>(const IndexTM<T> & a_iv);
   };

