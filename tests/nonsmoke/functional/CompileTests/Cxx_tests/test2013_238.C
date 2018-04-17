
namespace boost { 

struct na {};
template< typename param = na > struct next {};
template< typename Dummy = na > struct set0 {};

template< typename Set, typename Tail > struct s_iter;
template< typename Set > struct next< s_iter<Set,set0<> > > {};
template< typename Set, typename Tail > struct s_iter {};

// DQ (6/26/2013): This is where ROSE has a problem.
template< typename Set > struct s_iter<Set, set0<> > {};

}
