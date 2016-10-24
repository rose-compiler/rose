// CoderInfo-ootm.ii:5:11: error: dependent template scope name requires 'template' keyword

// this comes from gcc-2 header "stl-alloc.h"; I am going to
// recognize it and allow it

template < class _Tp, class _Allocator > 
struct _Alloc_traits
{
  typedef typename _Allocator::rebind < _Tp >::other allocator_type;
};
