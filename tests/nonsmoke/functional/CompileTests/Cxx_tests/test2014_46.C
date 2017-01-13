class ostream {};

template<typename T> class IndexTM;

template <typename T> ostream& operator<< (ostream  & a_os,const IndexTM<T> & a_iv);

template<typename T> 
class IndexTM
   {
     public:
          friend ostream& operator<< <>(ostream  & a_os,const IndexTM<T> & a_iv);
   };

