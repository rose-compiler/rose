#ifndef _GLIBCXX_TR1_MEMORY
#define _GLIBCXX_TR1_MEMORY 1

// #pragma GCC system_header

#if defined(_GLIBCXX_INCLUDE_AS_CXX11)
#  error TR1 header cannot be included from C++11 header
#endif

// #include <memory>
// #include <exception>        	// std::exception
// #include <typeinfo>         	// std::type_info in get_deleter


// #include <bits/stl_algobase.h>  // std::swap

#ifndef _STL_ALGOBASE_H
#define _STL_ALGOBASE_H 1

#if 0
#include <bits/c++config.h>
#include <bits/functexcept.h>
#include <bits/cpp_type_traits.h>
#include <ext/type_traits.h>
#include <ext/numeric_traits.h>
#endif

// #include <bits/stl_pair.h>

#ifndef _STL_PAIR_H
#define _STL_PAIR_H 1

// #include <bits/move.h> // for std::move / std::forward, and std::swap

#if __cplusplus >= 201103L
#include <type_traits> // for std::__decay_and_strip too
#endif

#endif


// #include <bits/stl_iterator_base_types.h>
// #include <bits/stl_iterator_base_funcs.h>
// #include <bits/stl_iterator.h>

// #include <bits/concept_check.h>
// #include <debug/debug.h>
// #include <bits/move.h> // For std::swap and _GLIBCXX_MOVE

#endif

#if 0
#include <iosfwd>           	// std::basic_ostream
#include <ext/atomicity.h>
#include <ext/concurrence.h>
#include <bits/functexcept.h>
#include <bits/stl_function.h> 	// std::less
#include <debug/debug.h>
#include <tr1/type_traits>
#include <tr1/shared_ptr.h>
#endif

#endif // _GLIBCXX_TR1_MEMORY
