#ifndef ROSE_BinaryAnalysis_ModelChecker_Exception_H
#define ROSE_BinaryAnalysis_ModelChecker_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>

#include <Rose/Exception.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Base class for model checker exceptions. */
class Exception: public Rose::Exception {
public:
    /** Construct an exception with an error message. */
    explicit Exception(const std::string &s)
        : Rose::Exception(s) {}

    ~Exception() throw() {}
};

/** Parse errors.
 *
 *  This exception is thrown if an error is encountered when parsing a file. Low-level syntax errors from YAML parsers throw
 *  parser-specific exceptions instead (such as YAML::Exception), while these @ref ParseError are thrown for higher level
 *  things, such as when we have a valid YAML file that doesn't contain the correct object description for the thing we're
 *  expecting to parse. */
class ParseError: public Exception {
public:
    /** Construct an exception with an error messsage. */
    ParseError(const boost::filesystem::path &fileName, const std::string &mesg)
        : Exception(fileName.empty() ? mesg : fileName.string() + ": " + mesg) {}

    ~ParseError() throw() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
