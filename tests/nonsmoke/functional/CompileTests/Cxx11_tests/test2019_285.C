template<class T1> class A_
   {
     public:
          template<class U> class B
             {
               public:
                    int f();
             };
   };

// Unparsed as: template<> class A_< int > ::B< double  > { };
template<> template<> class A_<int>::B<double> { };  // changed from WP

// Unparsed as: template<> int A_< char > ::B< char > ::f() { return 5; }
template<> template<> int A_<char>::B<char>::f() { return 5; }

void foobar()
   {
     A_<char>::B<char> b;
     b.f();
   }


