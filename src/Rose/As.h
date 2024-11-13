#ifndef ROSE_As_H
#define ROSE_As_H
#include <RoseFirst.h>

#include <Sawyer/SharedPointer.h>
#include <boost/shared_ptr.hpp>
#include <memory>

namespace Rose {

/** Dynamic pointer down-cast.
 *
 *  Down-casts the first argument from a pointer to `U` to a pointer to `T` where `T` is a subclass of `U`. If the cast is possible,
 *  then this function returns the new pointer, otherwise it returns a null pointer. Since pointers can be used in Boolean contexts,
 *  this function can also be used to test whether the cast is possible.
 *
 *  This function handles `std::shared_ptr`, `boost::shared_ptr`, `Sawyer::SharedPointer`, and raw pointers including Sage AST node
 *  pointers.
 *
 *  *Choice of name:* This function is named "as" because that name is also used in a number of other languages, including Perl,
 *  Ruby, PHP, and Rust (although Rust doesn't have dynamic casting). The name "as" is already used for a similar purpose in other
 *  parts of ROSE (although not for down casting). C# uses "is", and Sage AST nodes in ROSE also use "is" by prepending it to the
 *  type name. We decided against "is" because it implies a predicate whereas most of the cases where "is" is called in ROSE
 *  subsequently use the returned down-casted pointer -- it isn't just tested. And of course prepending "is" to the type name
 *  requires more code generation and we're trying to eliminate that. Java uses "instanceof" and the first implementation of this
 *  idea in ROSE used "dynamicPointerCast" (like the STL but with ROSE capitalization). Both of those were deemed too long for
 *  something that occurs as frequently as it does in the ROSE source code, even though many of those occurrences are C++ anti
 *  patterns.
 *
 *  Examples:
 *
 *  @code
 *  #include <Rose/As.h>
 *
 *  using Rose;
 *  using Rose::BinaryAnalysis;
 *
 *  Architecture::Base::Ptr arch = ...; // some kind of smart pointer
 *  if (as<Architecture::IntelI386>(base)) {
 *      std::cout <<"architecture is some implementation of Intel 80386\n";
 *  }
 *
 *  SgType *type = ...;
 *  if (auto array = as<SgArrayType>(type)) {         // equivalent to `isSgArrayType(node)`
 *      if (as<SgTypeBool>(array->get_base_type())) { // equivalent to `isSgTypeBool(node)`
 *          std::cout <<"the type is an array of Boolean\n";
 *      }
 *  }
 *  @endcode
 *
 *  @{ */
template<class T, class U>
std::shared_ptr<T>
as(const std::shared_ptr<U> &p) {
    return std::dynamic_pointer_cast<T>(p);
}

template<class T, class U>
boost::shared_ptr<T>
as(const boost::shared_ptr<U> &p) {
    return boost::dynamic_pointer_cast<T>(p);
}

template<class T, class U>
Sawyer::SharedPointer<T>
as(const Sawyer::SharedPointer<U> &p) {
    return p.template dynamicCast<T>();
}

template<class T, class U>
T*
as(U *p) {
    return dynamic_cast<T*>(p);
}
/** @} */

} // namespace

#endif
