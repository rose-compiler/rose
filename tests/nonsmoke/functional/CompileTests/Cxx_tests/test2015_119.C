
// class new_allocator { };
template<typename _Tp> class new_allocator { };

// template<typename _Tp> class allocator : public new_allocator { };
template<typename _Tp> class allocator : public new_allocator<_Tp> { };
  
extern template class allocator<char>;

