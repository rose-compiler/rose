#include <Rose/CommandLine/DurationParser.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_strtoull.h>

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <cerrno>
#include <string>

namespace Rose {
namespace CommandLine {

DurationParser::Ptr
DurationParser::instance() {
    return Ptr(new DurationParser);
}

DurationParser::Ptr
DurationParser::instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
    return Ptr(new DurationParser(valueSaver));
}

std::string
DurationParser::docString() {
    return ("A duration is specified as one or more non-negative decimal integers each followed by a multiplier letter. The "
            "letters are \"d\" for days, \"h\" for hours, \"m\" for minutes, and \"s\" for seconds. The letter \"s\" is "
            "optional when it appears at the end of a duration string. For example, \"1m30\" means on minute and 30 seconds.");
}

uint64_t
DurationParser::parse(const char *input, const char **rest) {
    ASSERT_not_null(input);
    const char *s = input;
    char *r = nullptr;

    while (isspace(*s))
        ++s;

    uint64_t total = 0;
    while (isdigit(*s)) {
        errno = 0;
        uint64_t n = rose_strtoull(s, &r, 10);
        if (r == s || ERANGE == errno)
            throw std::runtime_error("overflow when parsing duration");
        s = r;

        switch (*s) {
            case 'd':
                ++s;
                n *= 86400;
                break;
            case 'h':
                ++s;
                n *= 3600;
                break;
            case 'm':
                ++s;
                n *= 60;
                break;
            case 's':
                ++s;
                break;
            case '\0':
                break;
            default:
                if (!isspace(*s))
                    throw std::runtime_error("syntax error in duration");
                break;
        }
        total += n;
    }

    if (rest)
        *rest = s;
    return total;
}

uint64_t
DurationParser::parse(const std::string &str) {
    const char *s = str.c_str();
    const char *rest = nullptr;
    uint64_t seconds = parse(s, &rest);
    while (isspace(*rest))
        ++rest;
    if (*rest)
        throw std::runtime_error("extra text after end of duration: \"" + StringUtility::cEscape(rest) + "\"");
    return seconds;
}

Sawyer::CommandLine::ParsedValue
DurationParser::operator()(const char *input, const char **rest, const Sawyer::CommandLine::Location &loc) {
    uint64_t seconds = parse(input, rest);
    std::string parsed(input, *rest - input);
    return Sawyer::CommandLine::ParsedValue(seconds, loc, parsed, valueSaver());
}

std::string
DurationParser::toString(const Sawyer::Optional<uint64_t> &seconds) {
    if (seconds) {
        std::string retval;
        uint64_t s = *seconds;
        if (uint64_t d = s / 86400) {
            retval += boost::lexical_cast<std::string>(d) + "d";
            s -= d * 86400;
        }
        if (uint64_t h = s / 3600) {
            retval += boost::lexical_cast<std::string>(h) + "h";
            s -= h * 3600;
        }
        if (uint64_t m = s / 60) {
            retval += boost::lexical_cast<std::string>(m) + "m";
            s -= m * 60;
        }
        if (s > 0) {
            retval += boost::lexical_cast<std::string>(s) + "s";
        }

        return retval.empty() ? "0s" : retval;

    } else {
        return "ulimited";
    }
}

DurationParser::Ptr
durationParser() {
    return DurationParser::instance();
}

DurationParser::Ptr
durationParser(uint64_t &storage) {
    return DurationParser::instance(Sawyer::CommandLine::TypedSaver<uint64_t>::instance(storage));
}

DurationParser::Ptr
durationParser(Sawyer::Optional<uint64_t> &storage) {
    return DurationParser::instance(Sawyer::CommandLine::TypedSaver<Sawyer::Optional<uint64_t>>::instance(storage));
}

} // namespace
} // namespace
