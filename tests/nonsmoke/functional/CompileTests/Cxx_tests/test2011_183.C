namespace __detail
   {
     typedef char __one;
  // typedef char __two[2];

     template<typename _Tp> __one __test_type(int _Tp::*);
  // template<typename _Tp> __two& __test_type(...);
   }

