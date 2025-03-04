#ifndef ROSE_Affirm_H
#define ROSE_Affirm_H

#include <Rose/Exception.h>

namespace Rose {

/** Exceptions for value assertions. */
class AffirmationError: public Exception {
public:
    /** Construct error with message. */
    explicit AffirmationError(const std::string &mesg)
        : Exception(mesg) {}

    ~AffirmationError() throw() {}
};

// std::is_invocable backported for C++14
template<class F, class... Args>
struct is_invocable {
    template<class U>
    static auto test(U *p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());

    template<class U>
    static auto test(...) -> decltype(std::false_type());

    static constexpr bool value = decltype(test<F>(0))::value;
};

/** Test something about a value and then return it.
 *
 *  If the value satisifies the predicate, return the value, otherwise throw a @ref Rose::AffirmationError exception. If a @p
 *  message argument is specified then use it as the exception message instead of some generic string.
 *
 *  Example: Previously, to safely convert a signed value to an unsigned value of equal or larger size, one had to use a temporary
 *  variable like this:
 *
 * @code
 *  const auto temp = stream.tellp();
 *  ASSERT_require(temp >= 0);
 *  const size_t position = temp;
 * @endcode
 *
 *  Or better, use Boost's safe Numeric Conversion API which throws a non-ROSE exception:
 *
 * @code
 *  const auto position = boost::numeric_cast<size_t>(stream.tellp());
 * @endcode
 *
 *  With @c affirm we can do this:
 *
 * @code
 *  size_t p = affirm(f.tellp(), [](std::basic_ios::pos_type p) {return p >= 0;});
 * @endcode
 *
 *  Before you this to check the result from calling your own code, consider changing your called code to return a wrapped type such
 *  as @c Sawyer::Result<T,E> or @c Sawyer::Optional<T>. In that case, instead of testing the result in the caller like this:
 *
 * @code
 *  const size_t n = affirm(myIntParser(s), [](auto x) { return x != -1; }, "cannot parse input");
 * @endcode
 *
 *  You could rewrite @c myFunction to return `Sawyer::Result<int, std::string>` which is more flexible and can be used
 *  many ways, such as:
 *
 * @code
 *  const int i1 = myIntParser(s).orThrow(); // throw an error if s is not parsable
 *  const int i2 = myIntParser(s).expect("cannot parse input"); // throw an error if not parsed
 *  const int i3 = *myIntParser(s); // throw an error if not parsed
 *  const int i4 = myIntParser(s).orElse(911); // use 911 if not parsed
 *  if (myIntParser(s).contains(42)) // true if s was parsed as 42
 *
 *  int i5 = 911; // default
 *  if (myIntParser(s).assignTo(i5)) // true if s was parsed, with parsed value assigned to i5
 * @endcode
 *
 * @{ */
template<class Value, class Predicate>
typename std::enable_if<is_invocable<Predicate, Value>::value, Value&>::type
affirm(Value&& value, Predicate predicate) {
    if (predicate(value))
        return value;
    throw AffirmationError("predicate failed for value");
}

template<class Value, class Predicate>
typename std::enable_if<is_invocable<Predicate, Value>::value, Value&>::type
affirm(Value& value, Predicate predicate) {
    if (predicate(value))
        return value;
    throw AffirmationError("predicate failed for value");
}

template<class Value, class Predicate>
typename std::enable_if<is_invocable<Predicate, Value>::value, Value&>::type
affirm(Value&& value, Predicate predicate, std::string const& message) {
    if (predicate(value))
        return value;
    throw AffirmationError(message);
}

template<class Value, class Predicate>
typename std::enable_if<is_invocable<Predicate, Value>::value, Value&>::type
affirm(Value& value, Predicate predicate, std::string const& message) {
    if (predicate(value))
        return value;
    throw AffirmationError(message);
}
/** @} */

/** Test that a value evaluates to true in a Boolean context and return it.
 *
 *  This is the same as the version that takes a predicate, except the predicate is built-in and simply tests whether the value
 *  returns true in a Boolean context.
 *
 *  Example: Say we have a pointer to an @c SgAssignOp object and we don't know if it's a null pointer. If it's not a null pointer
 *  then we want to get the type of its right operand, which we also want to make sure is not null. Here's how ROSE was typically
 *  used:
 *
 * @code
 *   ROSE_ASSERT(ass != nullptr);
 *   SgExpression *temp1 = ass->get_rhs_operand();
 *   ROSE_ASSERT(temp1 != nullptr);
 *   SgType *type = temp1->get_type();
 *   ROSE_ASSERT(type != nullptr);
 * @endcode
 *
 *  This can now be expressed more succinctly and functionally as:
 *
 * @code
 *  SgType *type = affirm(affirm(affirm(ass)->get_rhs_operand())->get_type());
 * @endcode
 *
 *  For pointers, see also @ref notnull.
 *
 * @{ */
template<class Value>
Value&
affirm(Value&& value) {
    if (value)
        return value;
    throw AffirmationError("value does not evaluate to true in Boolean context");
}

template<class Value>
Value&
affirm(Value& value) {
    if (value)
        return value;
    throw AffirmationError("value does not evaluate to true in Boolean context");
}

template<class Value>
Value&
affirm(Value&& value, std::string const& message) {
    if (value)
        return value;
    throw AffirmationError(message);
}

template<class Value>
Value&
affirm(Value& value, std::string const& message) {
    if (value)
        return value;
    throw AffirmationError(message);
}
/** @} */

/** Check for non-null pointer.
 *
 *  This is just a wrapper around affirm that uses a better default error message of "null pointer".
 *
 *  Example: Say we have a pointer to an @c SgAssignOp object and we don't know if it's a null pointer. If it's not a null pointer
 *  then we want to get the type of its right operand, which we also want to make sure is not null. Here's how ROSE was typically
 *  used:
 *
 * @code
 *   ROSE_ASSERT(ass != nullptr);
 *   SgExpression *temp1 = ass->get_rhs_operand();
 *   ROSE_ASSERT(temp1 != nullptr);
 *   SgType *type = temp1->get_type();
 *   ROSE_ASSERT(type != nullptr);
 * @endcode
 *
 *  This can now be expressed more succinctly and functionally as:
 *
 * @code
 *  SgType *type = notnull(notnull(notnull(ass)->get_rhs_operand())->get_type());
 * @endcode
 *
 * @{ */
template<class Pointer>
Pointer&
notnull(Pointer&& pointer) {
    if (pointer)
        return pointer;
    throw AffirmationError("null pointer");
}

template<class Pointer>
Pointer&
notnull(Pointer& pointer) {
    if (pointer)
        return pointer;
    throw AffirmationError("null pointer");
}
/** @} */

} // namespace

// Convenience macro. The predicate should be a C++ Boolean expression in terms of a variable `x`. For example:
//
//    const int n = ROSE_AFFIRM(foo(a, b), x < 100); // will throw if foo(a,b) returns a value >= 100
//
#define ROSE_AFFIRM(VALUE, PREDICATE) ::Rose::affirm((VALUE), [](auto x) -> bool { return PREDICATE; })

#endif
