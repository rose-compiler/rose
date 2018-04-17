class Iterator {};

template<typename _Iterator>
class reverse_iterator
   {
     protected:
          Iterator current;

       // template<typename _Iter> reverse_iterator(const reverse_iterator<_Iter>& __x) : current(__x.base()) { }

          Iterator base() const { return current; }
   };
