#include <Rose/CommandLine/DurationParser.h>

#include <Rose/As.h>
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
    auto retval = Ptr(new DurationParser);
    retval
        ->with("d", 86400, "days")
        ->with("h", 3600, "hours")
        ->with("m", 60, "minutes")
        ->with("s", 1, "seconds");
    return retval;
}

DurationParser::Ptr
DurationParser::instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
    auto retval = Ptr(new DurationParser(valueSaver));
    retval
        ->with("d", 86400, "days")
        ->with("h", 3600, "hours")
        ->with("m", 60, "minutes")
        ->with("s", 1, "seconds");
    return retval;
}

std::string
DurationParser::docString() {
    return ("A duration is specified as one or more non-negative decimal integers each followed by a multiplier letter. The "
            "letters are \"d\" for days, \"h\" for hours, \"m\" for minutes, and \"s\" for seconds. The letter \"s\" is "
            "optional when it appears at the end of a duration string. For example, \"1m30\" means on minute and 30 seconds.");
}


std::string
DurationParser::toString(const Sawyer::Optional<uint64_t> &seconds) {
    if (seconds) {
        return Super::toString(*seconds);
    } else {
        return "unlimited";
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
