#ifndef ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <rose_strtoull.h>
#include <memory>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Flags and enums
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Update {
enum Flag { NO, YES };
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exceptions, errors, etc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Diagnostic facility for concolic testing. */
extern Sawyer::Message::Facility mlog;

// Internal: called by Rose::Diagnostics::initialize
void initDiagnostics();

/** Base class for exceptions for concolic testing. */
class Exception: public Rose::Exception {
public:
    explicit Exception(const std::string &mesg): Rose::Exception(mesg) {}
    ~Exception() throw () {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward references
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Specimen;
using SpecimenPtr = Sawyer::SharedPointer<Specimen>;

class TestCase;
using TestCasePtr = Sawyer::SharedPointer<TestCase>;

class ConcreteExecutor;
using ConcreteExecutorPtr = Sawyer::SharedPointer<ConcreteExecutor>;

class ExecutionEvent;
using ExecutionEventPtr = Sawyer::SharedPointer<ExecutionEvent>;

class LinuxExecutor;
using LinuxExecutorPtr = Sawyer::SharedPointer<LinuxExecutor>;

class LinuxI386Executor;
using LinuxI386ExecutorPtr = Sawyer::SharedPointer<LinuxI386Executor>;

class ConcolicExecutor;
using ConcolicExecutorPtr = Sawyer::SharedPointer<ConcolicExecutor>;

class TestSuite;
using TestSuitePtr = Sawyer::SharedPointer<TestSuite>;

class Database;
using DatabasePtr = Sawyer::SharedPointer<Database>;

class ExecutionManager;
using ExecutionManagerPtr = Sawyer::SharedPointer<ExecutionManager>;

class LinuxExitStatus;
using LinuxExitStatusPtr = Sawyer::SharedPointer<LinuxExitStatus>;

class ConcreteExecutorResult;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Database
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** ID class for database objects. */
template <class Tag>
class ObjectId: public Sawyer::Optional<size_t> {
public:
    using Value = size_t;                               /**< Type of ID. */
    using Super = Sawyer::Optional<Value>;              /**< Supertype of this class. */
    using Object = Tag;                                 /**< Type of object to which this ID refers. */
    using Pointer = Sawyer::SharedPointer<Tag>;         /**< Type of object pointer. */

    ObjectId() {}

    explicit
    ObjectId(const Value& v)
        : Super(v) {}

    ObjectId(const ObjectId& rhs)
        : Super(rhs) {}

    explicit ObjectId(const Sawyer::Optional<size_t> &id)
        : Super(id) {}

    /** Construct by parsing a string.
     *
     * This constructor creates an object ID by parsing it from a string. The string should consist of optional white space,
     * followed by a non-negative integer in C-style syntax (an optional radix prefix "0x", "0b", or "0", followed by digits
     * followed by optional additional white space. Syntactically incorrect input will result in throwing an @ref Exception. */
    explicit ObjectId(const std::string &s) {
        char *rest = NULL;
        uint64_t id = rose_strtoull(s.c_str(), &rest, 0);
        while (*rest && isspace(*rest)) ++rest;
        if (*rest)
            throw Exception("invalid syntax for object ID: \"" + StringUtility::cEscape(s) + "\"");
        try {
            *this = boost::numeric_cast<Value>(id);
        } catch (const boost::bad_numeric_cast&) {
            throw Exception("parsed object ID out of range: \"" + StringUtility::cEscape(s) + "\"");
        }
    }

    /** Assignment operator to replace default generated one. */
    ObjectId<Tag>& operator=(const ObjectId<Tag>& lhs) {
        this->Super::operator=(lhs);
        return *this;
    }

    /** Assignment overload to allow assignments of Value objects. */
    ObjectId<Tag>& operator=(const Value& v) {
        this->Super::operator=(v);
        return *this;
    }

    explicit operator bool() const {                    // because it's not explicit in the super class due to C++03 support
        return isEqual(Sawyer::Nothing()) ? false : true;
    }

    /** external operator to define ordering. */
    template<class _Tag>
    friend
    bool operator<(const ObjectId<_Tag>& lhs, const ObjectId<_Tag>& rhs);
};

/** defines ordering of ObjectIds. */
template<class Tag>
inline
bool operator<(const ObjectId<Tag>& lhs, const ObjectId<Tag>& rhs)
{
    if (!rhs) return false;
    if (!lhs) return true;

    return lhs.get() < rhs.get();
}

using TestSuiteId = ObjectId<TestSuite>;                /**< Database ID for test suite objects. */
using SpecimenId = ObjectId<Specimen>;                  /**< Database ID for specimen objects. */
using TestCaseId = ObjectId<TestCase>;                  /**< Database ID for test case objects. */
using ExecutionEventId = ObjectId<ExecutionEvent>;      /**< Database ID for execution event objects. */

/** Object traits.
 *
 *  T is the object type, such as TestSuite. */
template<class T>
struct ObjectTraits {
    using Id = void;
};

template<>
struct ObjectTraits<TestSuite> {
    using Id = TestSuiteId;
};

template<>
struct ObjectTraits<Specimen> {
    using Id = SpecimenId;
};

template<>
struct ObjectTraits<TestCase> {
    using Id = TestCaseId;
};

template<>
struct ObjectTraits<ExecutionEvent> {
    using Id = ExecutionEventId;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
