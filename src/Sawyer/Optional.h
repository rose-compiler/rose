// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_Optional_H
#define Sawyer_Optional_H

#include <Sawyer/Sawyer.h>

#include <stdexcept>

namespace Sawyer {

/** Represents no value.
 *
 *  An object of type Nothing can be assigned to any @ref Optional object to make it contain nothing.
 *
 * @code
 *  Optional<int> opt1;
 *  Optional<int> opt2(Nothing());
 *  Optional<int> opt3 = 123;
 *  opt3 = Nothing();
 * @endcode
 *
 * Anything can be assigned to @ref Nothing objects and nothing is stored:
 *
 * @code
 *  Nothing nothing(123); // still empty after this
 *  nothing = "testing";  // still empty after this
 * @endcode */
class Nothing {                                         // final
public:
    Nothing() {}
    template<class T> explicit Nothing(T) {}
    template<class T> Nothing& operator=(T) { return *this; }
    bool operator==(const Nothing&) const { return true; }
    bool operator!=(const Nothing&) const { return false; }
    bool operator>(const Nothing&) const { return false; }
    bool operator>=(const Nothing&) const { return true; }
    bool operator<(const Nothing&) const { return false; }
    bool operator<=(const Nothing&) const { return true; }
};

/** Holds a value or nothing.
 *
 *  This class is similar to boost::optional except simpler in order to avoid problems we were seeing with Microsoft
 *  compilers.
 *
 *  The stored value type (@ref Value) cannot be a reference type. */
template<typename T>
class Optional {
    alignas(alignof(T)) unsigned char data_[sizeof(T)]; // storage for the value
    bool isEmpty_;

    void *address() { return data_; }
    const void*address() const { return &data_; }

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s <<BOOST_SERIALIZATION_NVP(isEmpty_);
        if (!isEmpty_)
            s <<boost::serialization::make_nvp("value", get());
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        *this = Nothing();
        bool skip = false;
        s >>boost::serialization::make_nvp("isEmpty_", skip);
        if (!skip) {
            *this = T();
            s >>boost::serialization::make_nvp("value", get());
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        archive(cereal::make_nvp("isEmpty", isEmpty_));
        if (!isEmpty_)
            archive(cereal::make_nvp("value", get()));
    }

    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        *this = Nothing();
        bool isEmpty = false;
        archive(CEREAL_NVP(isEmpty));
        if (!isEmpty) {
            *this = T{};
            archive(cereal::make_nvp("value", get()));
        }
    }
#endif

public:
    /** Type of stored value. */
    typedef T Value;

    /** Default constructs nothing.
     *
     *  Constructs an optional value that points to nothing. The default constructor for @ref Value is not called. */
    Optional(): isEmpty_(true) {}

    /** Construct from value.
     *
     *  Constructs an optional object that holds a copy of @p v. */
    Optional(const Value &v): isEmpty_(false) {         // implicit
        new (address()) Value(v);                    // copy constructed in place
    }

    /** Construct from nothing.
     *
     *  Constructs an optional object that holds nothing.  The default constructor for @ref Value is not called. */
    Optional(const Nothing&): isEmpty_(true) {}

    /** Copy constructor.
     *
     *  Copy constructs an optional so that the new object has the same state (nothing or not) as the source object. If the
     *  source contains an object (not nothing) then the object is copy constructed into this optional. */
    Optional(const Optional &other) {
        isEmpty_ = other.isEmpty_;
        if (!isEmpty_) {
            const Value &otherValue = *other;
            new (address()) Value(otherValue);
       }
    }

    /** Destructor.
     *
     *  The destructor invokes the destructor for the value if a value is stored, otherwise it does nothing. */
    ~Optional() {
        if (!isEmpty_) {
            Value &thisValue = **this;
            thisValue.~Value();
        }
    }

    /** Value assignment.
     *
     *  Assigns the @p value to this optional. If this optional previously contained a value then the @ref Value assignment
     *  operator is used, otherwise the @ref Value copy constructor is used. */
    Optional& operator=(const Value &value) {
        if (isEmpty_) {
            new (address()) Value(value);
        } else {
            Value &thisValue = **this;
            thisValue = value;
        }
        isEmpty_ = false;
        return *this;
    }

    /** %Nothing assignment.
     *
     *  Assigns nothing to this optional.  If this optional previously contained a value then the value's destructor is
     *  called. */
    Optional& operator=(const Nothing&) {
        if (!isEmpty_) {
            Value &thisValue = **this;
            thisValue.~Value();
        }
        isEmpty_ = true;
        return *this;
    }
    
    /** Optional assignment.
     *
     *  Causes this optional to contain nothing or a copy of the @p other value depending on whether @p other contains a value
     *  or not.  If this didn't contain a value but @p other did, then the @ref Value copy constructor is used; if both
     *  contained a value then the @ref Value assignment operator is used; if this contained a value but @p other doesn't, then
     *  the @ref Value destructor is called. */
    Optional& operator=(const Optional &other) {
        if (isEmpty_ && !other.isEmpty_) {
            const Value &otherValue = *other;
            new (address()) Value(otherValue);
        } else if (!isEmpty_) {
            if (other.isEmpty_) {
                Value &thisValue = **this;
                thisValue.~Value();
            } else {
                Value &thisValue = **this;
                const Value &otherValue = *other;
                thisValue = otherValue;
            }
        }
        isEmpty_ = other.isEmpty_;
        return *this;
    }

    /** Reset as if default-constructed. */
    void reset() {
        *this = Nothing();
    }

    /** Dereference to obtain value.
     *
     *  If this optional contains a value then a reference to that value is returned. Otherwise an
     *  <code>std::domain_error</code> is thrown (the value is not in the container's domain).
     *
     *  @{ */
    const Value& operator*() const {
        return get();
    }
    Value& operator*() {
        return get();
    }
    const Value& get() const {
        if (isEmpty_)
            throw std::domain_error("dereferenced nothing");
        return *reinterpret_cast<const Value*>(address());
    }
    Value& get() {
        if (isEmpty_)
            throw std::domain_error("dereferenced nothing");
        return *reinterpret_cast<Value*>(address());
    }
    /** @} */

    /** Obtain a pointer to the value.
     *
     *  If this optional contains a value then a pointer to the value is returned. Otherwise an <code>std::domain_error</code>
     *  is thrown (the value is not in the container's domain).
     *
     *  @{ */
    const Value* operator->() const {
        return &get();
    }
    Value* operator->() {
        return &get();
    }
    /** @} */

    /** Obtain value or something else.
     *
     *  Returns a reference to the contained value if it exists, otherwise returns a reference to the argument.
     *
     * @code
     *  Object bar = ...;
     *  Object foo = objects.getOptional(key).orElse(bar);
     *  Optional<Object> baz = ...;
     *  std::cerr <<"baz is " <<baz.orElse(bar) <<"\n";
     * @endcode
     *
     *  @{ */
    const Value& orElse(const Value &dflt) const {
        return isEmpty_ ? dflt : **this;
    }
    const Value& orElse(Value &dflt) {
        return isEmpty_ ? dflt : **this;
    }
    const Optional orElse(const Optional &other) const {
        return isEmpty_ ? other : *this;
    }
    /** @} */

    /** If a value is present, return something else.
     *
     *  If this optional has a value, then return the argument, otherwise return nothing.
     *
     * @{ */
    Optional andThen(const Value &value) const {
        return isEmpty_ ? *this : Optional(value);
    }
    Optional andTHen(Value &value) const {
        return isEmpty_ ? *this : Optional(value);
    }
    const Optional& andThen(const Optional &other) const {
        return isEmpty_ ? *this : other;
    }
    /** @} */

    /** Obtain a value or a default.
     *
     *  Returns a copy of the contained value if it exists, otherwise returns a default constructed value.
     *  
     * @code
     *  Object bar = ...;
     *  Object foo = objects.getOptional(key).orDefault();
     *  Optional<Object> baz = ...;
     *  std::cerr <<"baz is " <<baz.orDefault() <<"\n";
     * @endcode
     */
    Value orDefault() const {
        return isEmpty_ ? Value() : **this;
    }

    /** Conditionally save a value.
     *
     *  If this optional object has a value then its value is written to @p out and this method returns true; otherwise the
     *  value of @p out is not changed and this method returns false.  This method is convenient to use in conditional
     *  statements like this:
     *
     * @code
     *  unsigned key = ...;
     *  std::string value;
     *  IntervalMap<Interval<unsigned>, std::string> imap = ...;
     *  while (imap.getOptional(key).assignTo(value)) ...
     * @endcode
     *
     *  where the alternative would be
     *
     * @code
     *  unsigned key = ...;
     *  IntervalMap<Interval<unsigned>, std::string> imap = ...;
     *  while (Optional<std::string> opt = imap.getOptional(key)) {
     *      std::string value = *opt;
     * @endcode */
    template<class U>
    bool assignTo(U &out) const {
        if (isEmpty_) {
            return false;
        } else {
            out = **this;
            return true;
        }
    }

    /** Compare two values.
     *
     *  Compares two optionals and returns true if they are both empty or if neither is empty and their values compare equal.
     *  This method should be used instead of <code>==</code>. The <code>==</code> operator is disabled because it is prone to
     *  misuse in the presense of implicit conversion to @c bool.
     *
     * @{ */
    bool isEqual(const Optional &other) const {
        return (isEmpty_ && other.isEmpty_) || (!isEmpty_ && !other.isEmpty_ && get()==other.get());
    }
    bool isEqual(const Value &other) const {
        return !isEmpty_ && get()==other;
    }
    bool isEqual(const Nothing&) const {
        return isEmpty_;
    }
    /** @} */

    /** Conditionally apply a functor @p f to the contents of an Optional.
     *
     *  If this optional object has a value then fmap builds a new Optional using @p f to construct the value.
     *  If it does not have a value, the new Optional is also empty.
     * */
    template <typename F>
    auto
    fmap(F&& f) -> Optional<decltype(f(get()))> {
        if (isEmpty_)
            return Optional<decltype(f(get()))>();
        return Optional<decltype(f(get()))>(f(get()));
    }

    
    // The following trickery is to allow things like "if (x)" to work but without having an implicit
    // conversion to bool which would cause no end of other problems. This is fixed in C++11.
private:
    typedef void(Optional::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    /** Type for Boolean context.
     *
     *  Implicit conversion to a type that can be used in a boolean context such as an <code>if</code> or <code>while</code>
     *  statement.  For instance:
     *
     *  @code
     *   Optional<int> x = 0;
     *   if (x) {
     *      //this is reached
     *   }
     *  @endcode */
    operator unspecified_bool() const {
        return isEmpty_ ? 0 : &Optional::this_type_does_not_support_comparisons;
    }
};


// These functions intentionally do not compile. They are to prevent comparisons and thus save users from making
// mistakes like this:
//    Optional<int> x = 0;
//    int y = 1;
//    if (x == y)       // won't compile
//    if (x && *x == y) // what they really meant
//    if (x.isEqual(y)) // another valid way to write it
template<typename T, typename U>
bool operator==(const Optional<T> &lhs, const U&) {
    lhs.this_type_does_not_support_comparisons();
    return false;
}

template<typename T, typename U>
bool operator!=(const Optional<T> &lhs, const U&) {
    lhs.this_type_does_not_support_comparisons();
    return false;
}

} // namespace
#endif
