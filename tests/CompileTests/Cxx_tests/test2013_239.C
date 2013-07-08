namespace boost { 

// Template declaration
template< typename T, typename param > struct next {};

// Partial specialization.
template< typename T > struct next<T,int> {};

}
