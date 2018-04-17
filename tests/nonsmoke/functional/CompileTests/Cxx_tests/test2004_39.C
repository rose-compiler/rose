#if 0
template<class _Tp, class _Ref, class _Ptr>
struct _List_iterator {
  typedef _List_iterator<_Tp,_Tp&,_Tp*>             iterator;

#if 0
  typedef _List_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;
  typedef _List_iterator<_Tp,_Ref,_Ptr>             _Self;
#endif

#if 0
// typedef bidirectional_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef _Ptr pointer;
  typedef _Ref reference;
//typedef _List_node<_Tp> _Node;
  typedef int _Node;
//typedef size_t size_type;
//typedef ptrdiff_t difference_type;

  _Node* _M_node;
#endif

#if 0
  _List_iterator(_Node* __x) : _M_node(__x) {}
  _List_iterator() {}
  _List_iterator(const iterator& __x) : _M_node(__x._M_node) {}

  bool operator==(const _Self& __x) const { return _M_node == __x._M_node; }
  bool operator!=(const _Self& __x) const { return _M_node != __x._M_node; }
  reference operator*() const { return (*_M_node)._M_data; }
#endif

#if 0
  pointer operator->() const { return &(operator*()); }

  _Self& operator++() { 
    _M_node = (_Node*)(_M_node->_M_next);
    return *this;
  }
  _Self operator++(int) { 
    _Self __tmp = *this;
    ++*this;
    return __tmp;
  }
  _Self& operator--() { 
    _M_node = (_Node*)(_M_node->_M_prev);
    return *this;
  }
  _Self operator--(int) { 
    _Self __tmp = *this;
    --*this;
    return __tmp;
  }
#endif
};
#endif



template<class _Tp, class _Ptr>
struct _List_iterator 
   {
     typedef _List_iterator<_Tp,_Tp*> iterator;
   };

_List_iterator<int,int> L;

