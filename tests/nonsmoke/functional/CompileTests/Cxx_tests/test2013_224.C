// #include <boost/mpl/set/aux_/set0.hpp>

template< typename Set, typename Tail > struct s_iter;

#if 0
template< typename Set, typename Tail > struct s_iter_get
    : eval_if< 
          has_key< Set,typename Tail::item_type_ >
        , identity< s_iter<Set,Tail> >
        , next< s_iter<Set,Tail> >
        >
{
};
#endif

template< typename Set, typename Tail > struct s_iter_impl
{
#if 0
    typedef Tail                        tail_;
    typedef forward_iterator_tag        category;
    typedef typename Tail::item_type_   type;
#endif
// #if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#if 0
    typedef typename s_iter_get< Set,typename Tail::base >::type next;
#endif
};


#if 0
template< typename Set, typename Tail > 
struct next< s_iter<Set,Tail> >
    : s_iter_get< Set,typename Tail::base >
{
};
#endif

template< typename S > 
struct set0
{
   S s;
};



template< typename Set > 
struct next< s_iter<Set,set0<> > >
{
    typedef s_iter<Set,set0<> > type;
};

template< typename Set, typename Tail > struct s_iter
    : s_iter_impl<Set,Tail>
{
};

template< typename Set > struct s_iter<Set, set0<> >
{
    typedef forward_iterator_tag category;
};

