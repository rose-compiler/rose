// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_Result_H
#define Sawyer_Result_H

#include <Sawyer/Optional.h>

#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <exception>
#include <string>
#include <type_traits>

namespace Sawyer {

template <class F, class... Args>
struct is_invocable
{
    template <class U>
    static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
    template <class U>
    static auto test(...) -> decltype(std::false_type());

    static constexpr bool value = decltype(test<F>(0))::value;
};


/** Success value. */
template<class T>
class Ok {
public:
    using Value = T;

private:
    Value ok_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(ok_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
        archive(cereal::make_nvp("ok", ok_));
    }
#endif

public:
    Ok() = delete;

    /** Copy constructor. */
    Ok(const Ok &other)
        : ok_(other.ok_) {}

    /** Construct from an value. */
    explicit Ok(const Value &ok)
        : ok_(ok) {}

    /** Assignment.
     *
     * @{ */
    Ok& operator=(const Ok &other) {
        ok_ = other.ok_;
        return *this;
    }
    Ok& operator=(const Value &ok) {
        ok_ = ok;
        return *this;
    }
    /** @} */

    /** Dereference to obtain value. */
    const Value& operator*() const {
        return ok_;
    }

    /** Dereference to obtain pointer. */
    const Value* operator->() const {
        return &ok_;
    }
};

// Specialization for Ok that stores string literals, as in Ok("foo"). These get treated as std::string instead.
template<size_t N>
class Ok<char[N]> {
public:
    using Value = std::string;

private:
    std::string ok_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(ok_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
        archive(cereal::make_nvp("ok", ok_));
    }
#endif

public:
    Ok() = delete;

    explicit Ok(const Value &s)
        : ok_(s) {}

    Ok& operator=(const Ok &other) {
        ok_ = other.ok_;
        return *this;
    }

    Ok& operator=(const Value &ok) {
        ok_ = ok;
        return *this;
    }

    const Value& operator*() const {
        return ok_;
    }

    const Value* operator->() const {
        return &ok_;
    }
};

/** Conventient way to constructo an @ref Ok value before C++17.
 *
 *  Template parameter type deduction in constructors first appeared in C++17. The standard library worked around this
 *  by using factory functions to instantiate these types without having to repeat the full type name. */
template<class T>
Ok<T> makeOk(const T &value) {
    return Ok<T>(value);
}

// Specialization for string literals, as in makeOk("foo") so they get treated as std::string instead.
inline Ok<std::string> makeOk(const char *s) {
    return Ok<std::string>(std::string(s));
}
inline Ok<std::string> makeOk(char *s) {
    return Ok<std::string>(std::string(s));
}

/** Error value. */
template<class E>
class Error {
public:
    using Value = E;

private:
    Value error_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(error_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
        archive(cereal::make_nvp("error", error_));
    }
#endif

public:
    Error() = delete;

    /** Copy constructor. */
    Error(const Error &other)
        : error_(other.error_) {}

    /** Construct from a value. */
    explicit Error(const E &error)
        : error_(error) {}

    /** Assignment.
     *
     * @{ */
    Error& operator=(const Error &other) {
        error_ = other.error_;
        return *this;
    }
    Error& operator=(const Value &error) {
        error_ = error;
        return *this;
    }
    /** @} */

    /** Dereference to obtain error. */
    const Value& operator*() const {
        return error_;
    }

    /** Dereference to obtain pointer to error. */
    const Value* operator->() const {
        return &error_;
    }
};

// Specialization for Error that stores string literals as in Error("syntax error"). It stores them as std::string instead.
template<size_t N>
class Error<char[N]> {
public:
    using Value = std::string;

private:
    std::string error_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(error_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
        archive(cereal::make_nvp("error", error_));
    }
#endif

public:
    Error() = delete;

    /** Copy constructor. */
    explicit Error(const Error &other)
        : error_(other.error_) {}

    /** Construct from a value. */
    explicit Error(const Value &error)
        : error_(error) {}

    /** Assignment.
     *
     * @{ */
    Error& operator=(const Error &other) {
        error_ = other.error_;
        return *this;
    }
    Error& operator=(const Value &error) {
        error_ = error;
        return *this;
    }
    /** @} */

    /** Dereference to obtain error. */
    const Value& operator*() const {
        return error_;
    }

    /** Dereference to obtain pointer to error. */
    const Value* operator->() const {
        return &error_;
    }
};

/** Conventient way to constructo an @ref Error value before C++17.
 *
 *  Template parameter type deduction in constructors first appeared in C++17. The standard library worked around this
 *  by using factory functions to instantiate these types without having to repeat the full type name. */
template<class T>
Error<T> makeError(const T &value) {
    return Error<T>(value);
}

// Specialization for string literals, as in makeError("foo") so they get treated as std::string instead.
inline Error<std::string> makeError(const char *s) {
    return Error<std::string>(std::string(s));
}
inline Error<std::string> makeError(char *s) {
    return Error<std::string>(std::string(s));
}

/** Result containing a value or an error. */
template<class T, class E>
class Result {
public:
    using OkValue = T;
    using ErrorValue = E;
    using OkType = Ok<T>;
    using ErrorType = Error<E>;

private:
    boost::variant<Ok<T>, Error<E>> result_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s <<boost::serialization::make_nvp("isOk", isOk());
        if (isOk()) {
            s <<boost::serialization::make_nvp("ok", unwrap());
        } else {
            s <<boost::serialization::make_nvp("error", unwrapError());
        }
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        bool isOk;
        s >>boost::serialization::make_nvp("isOk", isOk);
        if (isOk) {
            T ok;
            s >>boost::serialization::make_nvp("ok", ok);
            result_ = OkType(ok);
        } else {
            E error;
            s >>boost::serialization::make_nvp("error", error);
            result_ = ErrorType(error);
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        archive(cereal::make_nvp("isOk", isOk()));
        if (isOk()) {
            archive(cereal::make_nvp("ok", unwrap()));
        } else {
            archive(cereal::make_nvp("error", unwrapError()));
        }
    }

    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        bool isOk;
        archive(cereal::make_nvp("isOk", isOk));
        if (isOk) {
            T ok;
            archive(cereal::make_nvp("ok", ok));
            result_ = OkType(ok);
        } else {
            E error;
            archive(cereal::make_nvp("error", error));
            result_ = ErrorType(error);
        }
    }
#endif

public:
    template<class U = T>
    /*implicit*/ Result(const Ok<U> &ok)
        : result_(OkType(*ok)) {}

    template<class F = E>
    /*implicit*/ Result(const Error<F> &error)
        : result_(ErrorType(*error)) {}

    /** Assign an @ref Ok value to this result. */
    template<class U = T>
    Result& operator=(const Ok<U> &ok) {
        result_ = OkType(*ok);
        return *this;
    }

    /** Assign an @ref Error value to this result. */
    template<class F = E>
    Result& operator=(const Error<F> &error) {
        result_ = ErrorType(*error);
        return *this;
    }

    /** Test whether this result has the specified @ref Ok value. */
    template<class U = T>
    bool operator==(const Ok<U> &ok) const {
        return isOk() && *this->ok() == *ok;
    }

    /** Test whether this result does not have the specified @ref Ok value. */
    template<class U = T>
    bool operator!=(const Ok<U> &ok) const {
        return !(*this == ok);
    }

    /** Test whether this result has the specified @ref Error value. */
    template<class F = E>
    bool operator==(const Error<F> &error) const {
        return isError() && *this->error() == *error;
    }

    /** Test whether this result does not have the specified @ref Error value. */
    template<class F = E>
    bool operator!=(const Error<F> &error) const {
        return !(*this == error);
    }

    /** Test whether this result is equal to the other result. */
    template<class U, class F>
    bool operator==(const Result<U, F> &other) const {
        return ((isOk() && other.isOk() && *ok() == *other.ok()) ||
                (isError() && other.isError() && *error() == *other.error()));
    }

    /** Test whether this result is unequal to the other result. */
    template<class U, class F>
    bool operator!=(const Result<U, F> &other) const {
        return !(*this == other);
    }

    /** Returns true if the result is okay.
     *
     * @{ */
    bool isOk() const {
        return result_.which() == 0;
    }
    explicit operator bool() const {
        return isOk();
    }
    /** @} */

    /** Returns true if the result is an error. */
    bool isError() const {
        return !isOk();
    }

    /** Convert to Optional<T>.
     *
     *  If this result is okay, then return the result, otherwise return nothing. */
    const Sawyer::Optional<T> ok() const {
        if (isOk()) {
            return *boost::get<OkType>(result_);
        } else {
            return Sawyer::Nothing();
        }
    }

    /** Convert to Optional<E>.
     *
     *  If this result is an error, then return the error, otherwise return nothing. */
    const Sawyer::Optional<E> error() const {
        if (isOk()) {
            return Sawyer::Nothing();
        } else {
            return *boost::get<ErrorType>(result_);
        }
    }

    /** Returns the success value or throws an exception.
     *
     *  If this result is okay, then returns its value, otherwise throws an <code>std::runtime_error</code> with the specified string. */
    const T& expect(const std::string &mesg) const {
        if (isOk()) {
            return *boost::get<OkType>(result_);
        } else {
            throw std::runtime_error(mesg);
        }
    }

    /** Returns the success value or throws an exception.
     *
     *  If this result is okay, then returns its value, otherwise throws an <code>std::runtime_error</code>.
     *
     * @{ */
    const T& unwrap() const {
        return expect("result is not okay");
    }
    const T& operator*() const {
        return unwrap();
    }
    /** @} */

    /** Returns a pointer to the success value, or thows an exception.
     *
     *  If this result is okay, then returns a pointer to its value, otherwise throws an <code>std::runtime_error</code>. */
    const T* operator->() const {
        return &unwrap();
    }

    /** Returns the contained @ref Ok value or a provided default. */
    const T orElse(const T &dflt) const {
        return isOk() ? unwrap() : dflt;
    }

    /** Returns the contained @ref Ok value, or calls a function.
     *
     *  If this result is okay, then returns this result, otherwise calls the specified function, @p fn, with this
     *  result's @ref Error value and returns the function's result. */
    template<class Fn>
    typename std::enable_if<is_invocable<Fn, ErrorValue>::value, const Result>::type
    orElse(Fn fn) const {
        if (isOk()) {
            return *this;
        } else {
            return fn(*error());
        }
    }

    /** Returns this value or the other result.
     *
     *  If this result is okay then return it, otherwise returns the @p other result. */
    template<class F>
    const Result<T, F> orElse(const Result<T, F> &other) const {
        if (isOk()) {
            return boost::get<OkType>(result_);
        } else {
            return other;
        }
    }

    /** Returns the okay value or a default constructed value. */
    const T& orDefault() const {
        static T dflt = T();
        return isOk() ? unwrap() : dflt;
    }

    /** Returns the value or throws an exception.
     *
     *  If the result is okay, then its value is returned, otherwise either the error is thrown or it is used to construct the
     *  specified @p Exception which is then thrown. */
    template<class Exception = E>
    const T& orThrow() const {
        if (isOk()) {
            return unwrap();
        } else {
            throw Exception(*error());
        }
    }

    /** Returns the value or throws an exception constructed from the specified value. */
    template<class Exception = E>
    const T& orThrow(const Exception &e) const {
        if (isOk()) {
            return unwrap();
        } else {
            throw e;
        }
    }

    /** Returns the contained @ref Error value, or calls a function.
     *
     *  If this result is an error, then it's returned. Otherwise the okay value is passed to the specified function and that
     *  function's return value is returned. */
    template<class Fn>
    typename std::enable_if<is_invocable<Fn, OkValue>::value, const Result>::type
    andThen(Fn fn) const {
        if (isOk()) {
            return fn(*ok());
        } else {
            return *this;
        }
    }

    /** Returns this error or the other result.
     *
     *  If this result is okay, then returns @p other. Otherwise returns the error value of this result. */
    template<class U>
    const Result<U, E> andThen(const Result<U, E> &other) const {
        if (isOk()) {
            return other;
        } else {
            return boost::get<ErrorType>(result_);
        }
    }

    /** Returns the error value or throws an exception.
     *
     *  If this result is an error, then returns the error, otherwise throws an <code>std::runtime_error</code> with the specified string. */
    const E& expectError(const std::string &mesg) const {
        if (isOk()) {
            throw std::runtime_error(mesg);
        } else {
            return *boost::get<ErrorType>(result_);
        }
    }

    /** Returns the error value or throws an exception.
     *
     *  If this result is an error, then returns the error, otherwise throws an <code>std::runtime_error</code>. */
    const E& unwrapError() const {
        return expectError("result is not an error");
    }

    /** Returns true if this result contains the specified okay value. */
    template<class U>
    bool contains(const U &value) const {
        return isOk() ? unwrap() == value : false;
    }

    /** Returns true if this result contains the specified error value. */
    template<class F>
    bool containsError(const F &error) const {
        return isOk() ? false : unwrapError() == error;
    }

#if 0 // [Robb Matzke 2022-08-17]
    /** Transposes a @ref Result of an @ref Optional to an @ref Optional @ref Result.
     *
     *  If this result is an empty @ref Optional then an empty @ref Optional is returned. If this result is a non-empty @ref
     *  Optional then a non-empty @ref Optional containing the @ref Ok result is returned. Otherwise a non-empty @ref Optional
     *  containing the @ref Error is returned. */
    Optional<Result<typename T::Value, E>>
    transpose() const {
        if (isOk()) {
            if (unwrap().isEmpty()) {
                return Sawyer::Nothing();
            } else {
                return OkType(*unwrap());
            }
        } else {
            return ErrorType(unwrapError());
        }
    }

    /** Flatten a result of a result to a result. */
    Result<typename OkValue::OkValue, E> flatten() const {
        if (isOk()) {
            if (unwrap().isOk()) {
                return OkValue::OkType(unwrap().unwrap());
            } else {
                return ErrorType(unwrap().unwrapError());
            }
        } else {
            return ErrorType(unwrapError());
        }
    }
#endif

    /** Conditionally save a value.
     *
     *  If this result has a value, then assign it to the argument. Returns the result.
     *
     *
     * @{ */
    template<class U>
    const Result& assignTo(U &out) const {
        if (isOk())
            out = unwrap();
        return *this;
    }

    template<class U>
    Result& assignTo(U &out) const {
        if (isOk())
            out = unwrap();
        return *this;
    }
    /** @} */

    /** Conditionally apply a functor @p f to the contents of a Result.
     *
     *  If this object has a value then fmap builds a new Result using @p f to construct the value.
     *  If it does not have a value, the new Result keeps the error of the input.
     * */
    template <typename F>
    auto
    fmap(F&& f) -> Result<decltype(f(unwrap())), E> {
        if (isOk())
            return makeOk(f(unwrap()));
        return makeError(unwrapError());
    }

};

} // namespace
#endif
