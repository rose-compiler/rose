
// This is example of how ROSE might take the wrong branch comparied to EDG (we would have to turn off __EDG_VERSION_.
// From: #include <boost/mpl/aux_/config/gcc.hpp>

// Example code within BOOST
#if defined(__GNUC__) && !defined(__EDG_VERSION__)
#   define BOOST_MPL_CFG_GCC ((__GNUC__ << 8) | __GNUC_MINOR__)
#else
#   define BOOST_MPL_CFG_GCC 0
#endif
