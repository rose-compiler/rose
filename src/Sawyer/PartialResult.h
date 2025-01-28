// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_PartialResult_H
#define Sawyer_PartialResult_H

#include <Sawyer/Result.h>

#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <exception>
#include <string>
#include <type_traits>

namespace Sawyer {

/** Result containing a value and an error.
 *
 *  This type is similar to @ref Result except instead of containing either a value or an error it contains either a value or
 *  a value and an error.
 *
 *  Sometimes when a function fails it can still return a useful partial result, in which case it should return one of these
 *  types instead of @ref Result. */
template<class T, class E>
class PartialResult {
public:
    using OkValue = T;
    using ErrorValue = E;
    using OkType = Ok<T>;
    using ErrorType = Error<E>;

private:
    Ok<T> ok_;
    Optional<Error<E>> error_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) const {
        s <<BOOST_SERIALIZATION_NVP(ok_);
        s <<BOOST_SERIALIZATION_NVP(error_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) const {
        archive(CEREAL_NVP(ok_));
        archive(CEREAL_NVP(error_));
    }
#endif

public:
    template<class U = T>
    /*implicit*/ PartialResult(const Ok<U> &ok)
        : ok_(OkType(*ok)) {}

    template<class U = T, class F = E>
    PartialResult(const U &ok, const F &error)
        : ok_(OkType(ok)), error_(ErrorType(error)) {}

    template<class U = T, class F = E>
    PartialResult(const PartialResult<U, F> &other)
        : ok_(OkType(other.unwrap())) {
        if (other.isError())
            error_ = ErrorType(other.unwrapError());
    }

    /** Assign an @ref Ok value to this result. */
    template<class U = T>
    PartialResult& operator=(const Ok<U> &ok) {
        ok_ = OkType(*ok);
        return *this;
    }

    /** Assign another partial result to this result. */
    template<class U = T, class F = E>
    PartialResult& operator=(const PartialResult<U, F> &other) {
        ok_ = OkType(*other.ok_);
        if (other.error_) {
            error_ = ErrorType(*other.error_);
        } else {
            error_ = Sawyer::Nothing();
        }
    }

    /** Test whether this result has the specified @ref Ok value.
     *
     *  Regardless of whether this result is an error, its okay value is compared with the argument. If it is an error then
     *  the okay value might be partial, but is still compared. */
    template<class U = T>
    bool operator==(const Ok<U> &ok) const {
        return unwrap() == *ok;
    }

    /** Test whether this result does not have the specified @ref Ok value.
     *
     *  Regardless of whether this result is an error, its okay value is compared with the argument. If it is an error then
     *  the okay value might be partial, but is still compared. */
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
    bool operator==(const PartialResult<U, F> &other) const {
        return (*ok() == *other.ok() &&
                isError() == other.isError() &&
                (!isError() || *error() == *other.error()));
    }

    /** Test whether this result is unequal to the other result. */
    template<class U, class F>
    bool operator!=(const PartialResult<U, F> &other) const {
        return !(*this == other);
    }

    /** Returns true if the result is okay.
     *
     *  A result is okay if it has no error.
     *
     * @{ */
    bool isOk() const {
        return !isError();
    }
    explicit operator bool() const {
        return isOk();
    }
    /** @} */

    /** Returns true if the result has an error. */
    bool isError() const {
        return !!error_;
    }

    /** Convert to Optional<T>.
     *
     *  If this result is okay (has no error), then return the result, otherwise return nothing. */
    const Sawyer::Optional<T> ok() const {
        if (isOk()) {
            return *ok_;
        } else {
            return Sawyer::Nothing();
        }
    }

    /** Convert to Optional<E>.
     *
     *  If this result is an error, then return the error, otherwise return nothing. */
    const Sawyer::Optional<E> error() const {
        if (error_) {
            return **error_;
        } else {
            return Sawyer::Nothing();
        }
    }

    /** Returns the success value or throws an exception.
     *
     *  If this result is okay (no error), then returns its value, otherwise throws an <code>std::runtime_error</code> with the
     *  specified string. */
    const T& expect(const std::string &mesg) const {
        if (isOk()) {
            return *ok_;
        } else {
            throw std::runtime_error(mesg);
        }
    }

    /** Returns the (partial) success value.
     *
     *  Returns the success value. If this result has an error then the success value might be incomplete but will still be present.
     *
     * @{ */
    const T& unwrap() const {
        return *ok_;
    }
    const T& operator*() const {
        return unwrap();
    }
    /** @} */

    /** Returns a pointer to the (partial) success value.
     *
     *  If this result has an error then the success value might be incomplete, but it will still be present. */
    const T* operator->() const {
        return &unwrap();
    }

    /** Returns the contained @ref Ok value or a provided default.
     *
     *  If this result is an error then the default value is returned, otherwise the (complete) success value is returned. */
    const T orElse(const T &dflt) const {
        return isOk() ? unwrap() : dflt;
    }

    /** Returns the contained @ref Ok value, or calls a function.
     *
     *  If this result is okay (no error), then returns this result, otherwise calls the specified function, @p fn, with this
     *  result and returns the function's result. */
    template<class Fn>
    typename std::enable_if<is_invocable<Fn, const PartialResult>::value, const PartialResult>::type
    orElse(Fn fn) const {
        if (isOk()) {
            return *this;
        } else {
            return fn(*this);
        }
    }

    /** Returns this value or the other result.
     *
     *  If this result is okay (no error) then return it, otherwise returns the @p other result. */
    template<class F>
    const PartialResult<T, F> orElse(const PartialResult<T, F> &other) const {
        if (isOk()) {
            return ok_;
        } else {
            return other;
        }
    }

    /** Returns the okay value or a default constructed value. */
    const T& orDefault() const {
        static T dflt = T();
        return isOk() ? unwrap() : dflt;
    }

    /** Returns the success value or throws an exception.
     *
     *  If the result is okay (no error), then its value is returned, otherwise either the error is thrown or it is used to
     *  construct the specified @p Exception which is then thrown. */
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
    typename std::enable_if<is_invocable<Fn, OkValue>::value, const PartialResult>::type
    andThen(Fn fn) const {
        if (isOk()) {
            return fn(*ok());
        } else {
            return *this;
        }
    }

    /** Returns this (if error) or the other result.
     *
     *  If this result is an error it returns @p this, otherwise it returns @p other. */
    template<class U = T, class F = E>
    const PartialResult<U, F> andThen(const PartialResult<U, F> &other) const {
        if (isOk()) {
            return other;
        } else {
            return *this;
        }
    }

    /** Returns the error value or throws an exception.
     *
     *  If this result is an error, then returns the error, otherwise throws an <code>std::runtime_error</code> with the specified string. */
    const E& expectError(const std::string &mesg) const {
        if (error_) {
            return **error_;
        } else {
            throw std::runtime_error(mesg);
        }
    }

    /** Returns the error value or throws an exception.
     *
     *  If this result is an error, then returns the error, otherwise throws an <code>std::runtime_error</code>. */
    const E& unwrapError() const {
        return expectError("result is not an error");
    }

    /** Returns true if this result contains the specified (partial) okay value. */
    template<class U>
    bool contains(const U &value) const {
        return unwrap() == value;
    }

    /** Returns true if this result contains the specified error value. */
    template<class F>
    bool containsError(const F &error) const {
        return error_ ? unwrapError() == error : false;
    }

    /** Conditionally save a value.
     *
     *  If this result is not an error, then assign its okay value to the argument. Returns the result.
     *
     * @{ */
    template<class U>
    const PartialResult& assignTo(U &out) const {
        if (isOk())
            out = unwrap();
        return *this;
    }

    template<class U>
    PartialResult& assignTo(U &out) const {
        if (isOk())
            out = unwrap();
        return *this;
    }
    /** @} */

    /** Conditionally apply a functor @p f to the okay contents of a result.
     *
     *  The function is applied to the (partial) okay value to produce a new results. Any error is also copied to the result. */
    template <class F>
    auto
    fmap(F&& f) -> PartialResult<decltype(f(unwrap())), E> {
        using NewValue = decltype(f(unwrap()));
        using Retval = PartialResult<NewValue, E>;
        if (isOk()) {
            return Retval(Ok<NewValue>(f(unwrap())));
        } else {
            return Retval(f(unwrap()), unwrapError());
        }
    }
};

template<class T, class E>
PartialResult<T, E> makePartialResult(const T &ok, const E &error) {
    return PartialResult<T, E>(ok, error);
}

} // namespace
#endif
