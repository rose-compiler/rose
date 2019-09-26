template<class T> class A
   {
     public:
          template<class U> class B {};
   };

// Unparsed as: template<> class A_< int > ::B< double  > { };
template<> template<> class A<int>::B<double> {};  // changed from WP

