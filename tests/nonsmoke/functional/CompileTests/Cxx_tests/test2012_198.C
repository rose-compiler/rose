namespace XXX
   {

     template<typename _Iterator> struct reverse_iterator;

     template<typename _Iterator> void foo (const reverse_iterator<_Iterator>& __x);

  // The bug is that the first parameter is all that is significant and the name is not relevant to the type hashing...
     template<typename _IteratorL, typename _IteratorR> void foo (const reverse_iterator<_IteratorL>& __x);
   }

