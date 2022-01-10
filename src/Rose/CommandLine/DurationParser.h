#ifndef ROSE_CommandLine_DurationParser_H
#define ROSE_CommandLine_DurationParser_H

#include <Sawyer/CommandLine.h>
#include <Sawyer/Optional.h>
#include <string>

namespace Rose {
namespace CommandLine {

/** Command-line parser for durations. */
class DurationParser: public Sawyer::CommandLine::ValueParser {
protected:
    DurationParser() {}

    DurationParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}

public:
    /** Shared-ownership pointer to a @ref DurationParser. See @ref heap_object_shared_ownership. */
    using Ptr = Sawyer::SharedPointer<DurationParser>;

    /** Default allocating constructor. */
    static Ptr instance();

    /** Allocating constructor. */
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr&);

    /** Runtime documentation. */
    static std::string docString();

    /** Parse a duration from a C string.
     *
     *  Return value is in seconds parsed from the @p input string. If successful, then @p rest points to the
     *  first character beyond what was parsed. If a syntax error occurs, then an @c std::runtime_error is thrown. */
    static uint64_t parse(const char *input, const char **rest);

    /** Parse a duration from a C++ string.
     *
     *  Tries to parse a duration from the @p input string and returns the duration in seconds. The string may contain
     *  leading and trailing white space, but any extra characters will cause a syntax error. Syntax errors are reported by
     *  throwing a @c std::runtime_error. Since the underlying parsing is done on C strings, this function is ill-defined when
     *  the @p input contains NUL characters. */
    static uint64_t parse(const std::string &input);

    /** Convert seconds to a parsable string.
     *
     *  Instead of just returning the number of seconds, use days, hours, minutes, and seconds as appropriate. */
    static std::string toString(const Sawyer::Optional<uint64_t> &seconds);

private:
    virtual Sawyer::CommandLine::ParsedValue operator()(const char *input, const char **rest,
                                                        const Sawyer::CommandLine::Location&) override;
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
