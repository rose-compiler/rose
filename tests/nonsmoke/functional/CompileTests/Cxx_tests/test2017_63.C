
#include "boost/pool/pool.hpp"

#if 0

#include <boost/config.hpp>  // for workarounds

// std::less, std::less_equal, std::greater
#include <functional>
// new[], delete[], std::nothrow
#include <new>
// std::size_t, std::ptrdiff_t
#include <cstddef>
// std::malloc, std::free
#include <cstdlib>
// std::invalid_argument
#include <exception>
// std::max
#include <algorithm>

#include <boost/pool/poolfwd.hpp>

// boost::integer::static_lcm
#include <boost/integer/common_factor_ct.hpp>
// boost::simple_segregated_storage
#include <boost/pool/simple_segregated_storage.hpp>
// boost::alignment_of
#include <boost/type_traits/alignment_of.hpp>
// BOOST_ASSERT
#include <boost/assert.hpp>

#ifdef BOOST_POOL_INSTRUMENT
#include <iostream>
#include<iomanip>
#endif
#ifdef BOOST_POOL_VALGRIND
#include <set>
#include <valgrind/memcheck.h>
#endif

#endif
