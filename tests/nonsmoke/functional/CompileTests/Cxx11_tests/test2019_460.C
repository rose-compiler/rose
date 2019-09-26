#include <memory>

template < typename T >
class XXX
   {
     public :
       // std::shared_ptr<T> m_data;
       // int x;
#if 1
          void foobar ();
#else
          void foobar()
             {
            // DQ (2/14/2015): problem use of lamda expression in ROSE.
            // m_data = std::shared_ptr<T>(new T [dataSize()], [](T* p) { delete[] p;});
               m_data = std::shared_ptr<T>(new T [42], [](T* p) { delete[] p;});
             }
#endif
   };

void error()
   {
     XXX<int> a;
  // This line is required to demonstrate the bug in ROSE.
     a.foobar();
}
