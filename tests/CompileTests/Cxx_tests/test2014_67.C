template<typename T> class ABC;
template<typename T> int & operator<< ( int, const ABC<T> &);

template<typename T>
class ABC
   {
     public:
       // Original code with dependence on std::ostream.
       // friend std::ostream& operator<< <>( std::ostream& os, const ABC<T> & aRay );

       // Simpler example of error (without dependence on std::ostream):
          friend int & operator<< <>( int x, const ABC<T> & aRay );

       // More portable fix (but not quite the same semantics):
          template<typename S> friend int & operator<< ( int x, const ABC<S> & aRay);
   };
