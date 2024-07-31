#ifndef ROSE_CommandLine_IntervalParser_H
#define ROSE_CommandLine_IntervalParser_H

#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <rose_strtoull.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Interval.h>
#include <Sawyer/IntervalSet.h>
#include <string>

namespace Rose {
namespace CommandLine {

/** Parse an interval.
 *
 *  An interval is specified in one of the following forms, where N and M are non-negative integers:
 *
 * @li "N" is a singleton interval.
 * @li "N,M" is the min/max form specifying the two inclusive end points. M must not be less than N.
 * @li "N-M" is the begin/end form where M is greater than N and not included in the interval.
 * @li "N+M" is the base/size form where N is the minimum value and M is the number of items.
 *
 * The integers can be specified in decimal, octal, hexadecimal, or binary using the usual C/C++ syntax. */
template<class Interval>
class IntervalParser: public Sawyer::CommandLine::ValueParser {
protected:
    IntervalParser() {}

    IntervalParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}

public:
    /** Shared-ownership pointer.
     *
     *  See @ref heap_object_shared_ownership. */
    using Ptr = Sawyer::SharedPointer<IntervalParser>;

    /** Default allocating constructor. */
    static Ptr instance() {
        return Ptr(new IntervalParser);
    }

    /** Allocating constructor. */
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
        return Ptr(new IntervalParser(valueSaver));
    }

    /** Runtime documentation. */
    static std::string docString(const std::string &interval = "interval", const std::string &value = "value") {
        return ("The " + interval + " can be specified in a number of forms:"
                " a single " + value + " represents a singleton " + interval + ";"
                " a first and inclusive last " + value + " separated by a comma;"
                " a begin and exclusive end " + value + " separated by a hyphen;"
                " a begin " + value + " and count separated by a plus sign;"
                " the word \"all\" represents the universal " + interval + ";"
                " and the word \"empty\" or an empty string represents the empty " + interval + "."

                " When the " + interval + " is specified as a range, the first " + value + " must be less than or equal to"
                " the second value. The " + value + " can be specified in decimal, hexadecimal (leading \"0x\"),"
                " octal (leading \"0\"), or binary (leading \"0b\"). The upper " + value + " can be the word \"max\" when"
                " appearing after a comma.");
    }

    /** Parse an interval from a C string.
     *
     *  Tries to parse an interval from the @p input string, and if successful adjusts @p rest to point to the first character
     *  beyond what was parsed. If a syntax error occurs, then an @c std::runtime_error is thrown. */
    static Interval parse(const char *input, const char **rest) {
        const char *s = input;
        char *r = nullptr;
        bool hadRangeError = false, isEmpty = false;
        while (isspace(*s)) ++s;

        if (!strcmp(s, "all")) {
            *rest = s + 3;
            return Interval::whole();
        }

        if (!strcmp(s, "empty")) {
            *rest += 5;
            return Interval();
        }

        // Minimum
        errno = 0;
        uint64_t least64 = rose_strtoull(s, &r, 0);
        if (r == s)
            throw std::runtime_error("unsigned integer expected for interval minimum");
        if (ERANGE == errno)
            hadRangeError = true;
        typename Interval::Value least{};
        try {
            least = boost::numeric_cast<typename Interval::Value>(least64);
        } catch (const boost::bad_numeric_cast&) {
            hadRangeError = true;
        }
        s = r;

        // Maximum, end, size, or nothing
        typename Interval::Value greatest = least;
        while (isspace(*s)) ++s;
        if (',' == *s) {                                // ',' means a max value is specified
            ++s;
            if (0 == strncmp(s, "max", 3)) {
                greatest = Interval::whole().greatest();
                s += 3;
                r = const_cast<char*>(s);
            } else {
                errno = 0;
                uint64_t greatest64 = rose_strtoull(s, &r, 0);
                if (r == s)
                    throw std::runtime_error("unsigned integer expected for interval maximum");
                if (ERANGE == errno)
                    hadRangeError = true;
                try {
                    greatest = boost::numeric_cast<typename Interval::Value>(greatest64);
                } catch (const boost::bad_numeric_cast&) {
                    hadRangeError = true;
                }
                s = r;
            }
        } else if ('-' == *s) {                         // '-' means an exclusive end address is specified (think "-" 1)
            ++s;
            errno = 0;
            uint64_t greatest64 = rose_strtoull(s, &r, 0);
            if (r == s)
                throw std::runtime_error("unsigned integer expected for interval end");
            if (ERANGE == errno)
                hadRangeError = true;
            try {
                greatest = boost::numeric_cast<typename Interval::Value>(greatest64);
            } catch (const boost::bad_numeric_cast&) {
                hadRangeError = true;
            }
            if (greatest == least)
                isEmpty = true;
            --greatest;
            s = r;
        } else if ('+' == *s) {                         // '+' means a size follows (zero is allowed)
            ++s;
            errno = 0;
            uint64_t size64 = rose_strtoull(s, &r, 0);
            if (r == s)
                throw std::runtime_error("unsigned integer expected for interval size");
            if (ERANGE == errno)
                hadRangeError = true;
            typename Interval::Value size{};
            try {
                size = boost::numeric_cast<typename Interval::Value>(size64);
            } catch (const boost::bad_numeric_cast&) {
                hadRangeError = true;
            }
            if (0 == size)
                isEmpty = true;
            greatest = least + size - 1;
            s = r;
        } else if (!*s) {                               // end-of-string means the interval is a singleton
            /*void*/
        }

        // Successful parsing?
        *rest = r;
        std::string parsed(input, *rest - input);
        if (hadRangeError)
            throw std::range_error("overflow when parsing \"" + parsed + "\"");
        if (greatest < least)
            throw std::range_error("interval seems backward: \"" + parsed + "\"");

        if (!isEmpty) {
            return Interval::hull(least, greatest);
        } else {
            return Interval();
        }
    }

    /** Parse an interval from a C++ string.
     *
     *  Tries to parse an interval from the @p input string. The string may contain leading and trailing white space, but any
     *  extra characters will cause a syntax error. Syntax errors are reported by throwing @c std::runtime_error.  Since the
     *  underlying parsing is done on C strings, this function is ill-defined when the @p input contains NUL characters. */
    static Interval parse(const std::string &input) {
        const char *s = input.c_str();
        const char *rest = nullptr;
        Interval retval = parse(s, &rest);
        while (isspace(*rest)) ++rest;
        if (*rest)
            throw std::runtime_error("extra text after end of interval specification: \"" + StringUtility::cEscape(rest) + "\"");
        return retval;
    }

    /** Unparse an interval to a string. */
    static std::string toString(const Interval &interval) {
        if (interval.isEmpty()) {
            return "empty";
        } else if (interval == Interval::whole()) {
            return "all";
        } else if (interval.least() == interval.greatest()) {
            if (interval.least() < 256) {
                return boost::lexical_cast<std::string>(interval.least());
            } else {
                return StringUtility::addrToString(interval.least());
            }
        } else {
            if (interval.greatest() < 256) {
                return boost::lexical_cast<std::string>(interval.least()) + "," +
                    boost::lexical_cast<std::string>(interval.greatest());
            } else {
                return StringUtility::addrToString(interval.least()) + "," +
                    StringUtility::addrToString(interval.greatest());
            }
        }
    }

private:
    virtual Sawyer::CommandLine::ParsedValue operator()(const char *input, const char **rest,
                                                        const Sawyer::CommandLine::Location &loc) override {
        Interval val = parse(input, rest);
        std::string parsed(input, *rest - input);
        return Sawyer::CommandLine::ParsedValue(val, loc, parsed, valueSaver());
    }
};

template<class Interval>
typename IntervalParser<Interval>::Ptr intervalParser(Interval &storage) {
    return IntervalParser<Interval>::instance(Sawyer::CommandLine::TypedSaver<Interval>::instance(storage));
}

template<class Interval>
typename IntervalParser<Interval>::Ptr intervalParser(std::vector<Interval> &storage) {
    return IntervalParser<Interval>::instance(Sawyer::CommandLine::TypedSaver<std::vector<Interval>>::instance(storage));
}

template<class Interval>
typename IntervalParser<Interval>::Ptr intervalParser(Sawyer::Container::IntervalSet<Interval> &storage) {
    return IntervalParser<Interval>
        ::instance(Sawyer::CommandLine::TypedSaver<Sawyer::Container::IntervalSet<Interval>>::instance(storage));
}

template<class Interval>
typename IntervalParser<Interval>::Ptr intervalParser() {
    return IntervalParser<Interval>::instance();
}

} // namespace
} // namespace

#endif
