#ifndef ROSE_CommandLine_SuffixMultiplierParser_H
#define ROSE_CommandLine_SuffixMultiplierParser_H

#include <Rose/As.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_strtoull.h>

#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <map>
#include <Sawyer/CommandLine.h>
#include <string>

namespace Rose {
namespace CommandLine {

/** Parse values followed by unit names.
 *
 *  Parses one or more multiplicative terms and sums them to obtain a numeric value. A term is a number followed by an optional
 *  unit name. For instance, a time duration could have units "h" (hours), "m" (minutes), and "s" (seconds) and be specified as
 *  "3h2m1s" where the terms are "3h" (3*3600) plus "2m" (2*60) plus "1s" (1*1) for a total of 10921.
 *
 *  The numbers used in the terms are parsed as either integers or floating point depending on the template argument. For
 *  integers, the usual C/C++ syntax for hexadecimal, octal, and binary parsing are available if configured, and floating point
 *  exponential parsing is also conditionally available.
 *
 *  The unit names and multipliers are specified by the user. Any number of names and multipliers are allowed, and longer names
 *  are matched preferentially before shorter names so that, for example, "hour" is parsed before "h". One of the unit names
 *  my also be the empty string, which can only apply to the final term. */
template<class T>
class SuffixMultiplierParser: public Sawyer::CommandLine::ValueParser {
public:
    enum class Preferred { NO, YES };

private:
    struct Suffix {
        T multiplier;
        Preferred preferred;
    };

    using Suffixes = std::map<std::string /*name*/, Suffix>;
    Suffixes suffixes_;
    bool extendedSyntax_ = false;

protected:
    SuffixMultiplierParser() {}

    SuffixMultiplierParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}

public:
    /** Shared-ownership pointer to a @ref Rose::CommandLine::SuffixMultiplierParser.
     *
     *  See @ref heap_object_shared_ownership. */
    using Ptr = Sawyer::SharedPointer<SuffixMultiplierParser>;

    /** Default allocating constructor. */
    static Ptr instance() {
        return Ptr(new SuffixMultiplierParser);
    }

    /** Allocating constructor. */
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
        return Ptr(new SuffixMultiplierParser(valueSaver));
    }

    /** Insert a suffix definition.
     *
     *  If @p preferred is set, then this suffix can be used when converting a value to a string.  Alternatively, all suffixes
     *  with the same multiplier can be specified in a single @ref with call and the first one can be used in string
     *  conversion.
     *
     * @{ */
    Ptr with(const std::string &suffix, T multiplier, Preferred preferred = Preferred::YES) {
        suffixes_[suffix] = Suffix{.multiplier = multiplier, .preferred = preferred};
        return as<SuffixMultiplierParser>(sharedFromThis());
    }

    Ptr with(const std::string &suffix, T multiplier, const std::string &alias1, const std::string &alias2 = "",
             const std::string &alias3 = "", const std::string &alias4 = "") {
        suffixes_[suffix] = Suffix{multiplier, Preferred::YES};
        if (!alias1.empty())
            suffixes_[alias1] = Suffix{multiplier, Preferred::NO};
        if (!alias2.empty())
            suffixes_[alias2] = Suffix{multiplier, Preferred::NO};
        if (!alias3.empty())
            suffixes_[alias3] = Suffix{multiplier, Preferred::NO};
        if (!alias4.empty())
            suffixes_[alias4] = Suffix{multiplier, Preferred::NO};
        return as<SuffixMultiplierParser>(sharedFromThis());
    }
    /** @} */

    /** Property: Allow extended syntax for numberic values.
     *
     *  If clear, then integers and floating-point value syntax is simple. For integers, the syntax is a sequence of one or
     *  more digits interpreted as a decimal integer; and for floating point the syntax is a simple integer followed by an
     *  optional decimal point and simple integer.
     *
     *  If set, then integers and floating-point values have a more complicated syntax. Integers can be specified as
     *  hexadecimal with a leading "0x", binary with a leading "0b", octal with a leading "0" (not followed by "x" or "b"), and
     *  decimal in other cases. Care must be taken with hexadecimal since the letters "a" through "f" (lower and upper case)
     *  would be interpreted as part of the integer rather than the beginning of a suffix unit name. Extended floating-point
     *  syntax is a simple floating point value followed by an optional exponent. An exponent is the letter "e" or "E", an
     *  optional "+" or "-" sign, followed by an integer.
     *
     *  The default is to use simple syntax.
     *
     *  @{ */
    bool extendedSyntax() const {
        return extendedSyntax_;
    }
    Ptr extendedSyntax(bool b) {
        extendedSyntax_ = b;
        return as<SuffixMultiplierParser>(sharedFromThis());
    }
    /** @} */

    /** Parse from a C string. */
    T parse(const char *input, const char **rest) {
        const char *s = input;
        const char *r = nullptr;
        T total = 0;

        while (isspace(*s)) ++s;

        while (*s) {
            T n = parseNumber(s, &r, T{});              // throws if not possible
            s = r;

            while (*r && !isdigit(*r))
                ++r;
            if (s == r)
                break;
            std::string suffix(s, r);
            auto found = suffixes_.find(suffix);
            if (found == suffixes_.end()) {
                total += n;
                r = const_cast<char*>(s);
                break;
            } else {
                total += n * found->second.multiplier;
                s = r;
            }
        }

        if (rest)
            *rest = r;
        return total;
    }

    /** Parse from a C++ string. */
    T parse(const std::string &input) {
        const char *s = input.c_str();
        const char *rest = nullptr;
        T retval = parse(s, &rest);
        while (isspace(*rest)) ++rest;
        if (*rest)
            throw std::runtime_error("extra text after end of interval specification: \"" + StringUtility::cEscape(rest) + "\"");
        return retval;
    }

    static std::pair<T, T> quotientRemainder(T product, T divisor, uint64_t) {
        uint64_t q64 = product / divisor;
        uint64_t r64 = product % divisor;
        try {
            T q = boost::numeric_cast<T>(q64);
            T r = boost::numeric_cast<T>(r64);
            return {q, r};
        } catch (const boost::bad_numeric_cast&) {
            throw std::runtime_error("integer overflow");
        }
    }

    static std::pair<T, T> quotientRemainder(T product, T divisor, double) {
        if (product < divisor) {
            return {0, product};
        } else {
            double qd = std::floor(product / divisor);
            double rd = std::fmod(product, divisor);
            try {
                T q = boost::numeric_cast<T>(qd);
                T r = boost::numeric_cast<T>(rd);
                return {q, r};
            } catch (const boost::bad_numeric_cast&) {
                throw std::runtime_error("floating-point overflow");
            }
        }
    }

    /** Unparse to a string. */
    std::string toString(T value) {
        // Get a list of preferred suffixes sorted by increasing multipliers. When two or more preferred suffixes have the same
        // multiplier, use the one with the longest name. If name lengths tie, choose one arbitrarily. We do this by first
        // sorting the suffixes according to increasing multiplier and decreasing name length; then removing duplicates based
        // on multipliers only.
        using Pair = std::pair<std::string, Suffix>;
        std::vector<Pair> byValue(suffixes_.begin(), suffixes_.end());
        byValue.erase(std::remove_if(byValue.begin(), byValue.end(),
                                     [](const Pair &a) {
                                         return a.second.preferred != Preferred::YES;
                                     }),
                      byValue.end());
        std::sort(byValue.begin(), byValue.end(),
                  [](const Pair &a, const Pair &b) {
                      if (a.second.multiplier != b.second.multiplier)
                          return a.second.multiplier < b.second.multiplier;
                      if (a.first.size() != b.first.size())
                          return a.first.size() > b.first.size();
                      return a.first < b.first;
                  });
        byValue.erase(std::unique(byValue.begin(), byValue.end(),
                                   [](const Pair &a, const Pair &b) {
                                       return a.second.multiplier == b.second.multiplier;
                                   }),
                       byValue.end());

        // Form the output by trying successively smaller suffixes.
        std::string retval;
        while (value > 0) {
            while (!byValue.empty() && byValue.back().second.multiplier > value)
                byValue.pop_back();
            if (byValue.empty())
                break;

            std::pair<T, T> qr = quotientRemainder(value, byValue.back().second.multiplier, T{});
            assert(qr.first > 0);
            retval += boost::lexical_cast<std::string>(qr.first) + byValue.back().first;
            value = qr.second;
        }

        if (value > 0)
            retval += boost::lexical_cast<std::string>(value);
        if (retval.empty())
            retval += "0";
        return retval;
    }

private:
    virtual Sawyer::CommandLine::ParsedValue operator()(const char *input, const char **rest,
                                                        const Sawyer::CommandLine::Location &loc) override {
        T val = parse(input, rest);
        std::string parsed(input, *rest - input);
        return Sawyer::CommandLine::ParsedValue(val, loc, parsed, valueSaver());
    }

    // Parse integer literal w/out leading white space
    T parseNumber(const char *input, const char **rest, uint64_t) {
        assert(input);
        assert(rest);
        char *r = nullptr;
        errno = 0;
        uint64_t n = rose_strtoull(input, &r, extendedSyntax_ ? 0 : 10);
        if (input == *rest)
            throw std::runtime_error("unsigned integer expected");
        if (ERANGE == errno)
            throw std::runtime_error("integer magnitude is too large");
        try {
            T retval = boost::numeric_cast<T>(n);
            *rest = r;
            return retval;
        } catch (const boost::bad_numeric_cast&) {
            throw std::runtime_error("integer magnitude is too large");
        }
    }

    // Parse floating point literal w/out leading white space
    T parseNumber(const char *input, const char **rest, double) {
        assert(input);
        assert(rest);

        // Find the end of the significand (digits followed by optional decimal point and more digits). We don't worry about
        // how many digits there are on either side of the decimal point since strtod will do more strict checking later.
        const char *significandEnd = input;
        bool hadDecimalPoint;
        while (isdigit(*significandEnd) || ('.' == *significandEnd && !hadDecimalPoint)) {
            if ('.' == *significandEnd)
                hadDecimalPoint = true;
            ++significandEnd;
        }

        // For extended syntax, find the end of the optional exponent: [eE][-+]?\d+. Again, we don't have to be too precise
        // because strtod will do the strict checking. However, we should be careful that the "e" is followed by at least
        // one digit so as not to be confused by suffixes that start with "e" (other than the suffix "e" itself which will
        // be ambiguous when followed immediately by a digit).
        const char *end = significandEnd;
        if (extendedSyntax_ && ('e' == *end || 'E' == *end)) {
            ++end;
            if ('+' == *end || '-' == *end)
                ++end;
            if (isdigit(*end)) {
                while (isdigit(*end)) ++end;
            } else {
                end = significandEnd;
            }
        }

        const std::string toParse(input, end);
        const char *s = toParse.c_str();
        char *r = nullptr;
        errno = 0;
        double d = strtod(s, &r);
        if (s == r)
            throw std::runtime_error("floating-point number expected");
        if (ERANGE == errno)
            throw std::runtime_error("floating-point value is out of range");
        try {
            T retval = boost::numeric_cast<T>(d);
            *rest = input + (r - s);
            return retval;
        } catch (const boost::bad_numeric_cast&) {
            throw std::runtime_error("floating-point value is out of range");
        }
    }
};

template<class T>
typename SuffixMultiplierParser<T>::Ptr suffixMultiplierParser(T &storage) {
    return SuffixMultiplierParser<T>::instance(Sawyer::CommandLine::TypedSaver<T>::instance(storage));
}

template<class T>
typename SuffixMultiplierParser<T>::Ptr suffixMultiplierParser(std::vector<T> &storage) {
    return SuffixMultiplierParser<T>::instance(Sawyer::CommandLine::TypedSaver<std::vector<T>>::instance(storage));
}

template<class T>
typename SuffixMultiplierParser<T>::Ptr suffixMultiplierParser() {
    return SuffixMultiplierParser<T>::instance();
}

} // namespace
} // namespace

#endif
