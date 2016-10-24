struct iterator_type {};

template<typename _Iterator>
class reverse_iterator
   {
     public:
          reverse_iterator() { }

          explicit reverse_iterator(iterator_type __x) { }

          reverse_iterator(const reverse_iterator& __x) { }

          template<typename _Iter> reverse_iterator(const reverse_iterator<_Iter>& __x) { }
   };



