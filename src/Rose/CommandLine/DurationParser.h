#ifndef ROSE_CommandLine_DurationParser_H
#define ROSE_CommandLine_DurationParser_H

#include <Rose/CommandLine/SuffixMultiplierParser.h>

namespace Rose {
namespace CommandLine {

/** Command-line parser for durations. */
class DurationParser: public SuffixMultiplierParser<uint64_t> {
    using Super =            SuffixMultiplierParser<uint64_t>;
protected:
    DurationParser() {}

    DurationParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Super(valueSaver) {}

public:
    /** Shared-ownership pointer to a @ref DurationParser. See @ref heap_object_shared_ownership. */
    using Ptr = Sawyer::SharedPointer<DurationParser>;

    /** Default allocating constructor. */
    static Ptr instance();

    /** Allocating constructor. */
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr&);

    /** Runtime documentation. */
    static std::string docString();

    /** Convert seconds to a parsable string.
     *
     *  Instead of just returning the number of seconds, use days, hours, minutes, and seconds as appropriate. */
    std::string toString(const Sawyer::Optional<uint64_t> &seconds);
};

/** Constructs a @ref DurationParser.
 *
 *  When the parser runs, it stores the parsed result in @p storage.
 *
 * @{ */
DurationParser::Ptr durationParser(uint64_t &storage);
DurationParser::Ptr durationParser(Sawyer::Optional<uint64_t> &storage);
/** @} */

/** Constructs a @ref DurationParser. */
DurationParser::Ptr durationParser();

} // namespace
} // namespace

#endif
