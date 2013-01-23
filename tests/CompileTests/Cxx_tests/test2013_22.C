namespace std
   {
   }


namespace std
   {
     class A {};
     class B {};
      
     template<class T, class S> class X { static S foobar; };

     template<class T, class S> S X<T,S>::foobar;

     template<> A X<A,A>::foobar;
     template<> B X<A,B>::foobar;
   }

