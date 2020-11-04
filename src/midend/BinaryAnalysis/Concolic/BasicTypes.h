#ifndef ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <rose_strtoull.h>

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
typedef Sawyer::SharedPointer<Specimen> SpecimenPtr;

class TestCase;
typedef Sawyer::SharedPointer<TestCase> TestCasePtr;

class SystemCall;
typedef Sawyer::SharedPointer<SystemCall> SystemCallPtr;

class ConcreteExecutor;
typedef Sawyer::SharedPointer<ConcreteExecutor> ConcreteExecutorPtr;

class LinuxExecutor;
typedef Sawyer::SharedPointer<LinuxExecutor> LinuxExecutorPtr;

class ConcolicExecutor;
typedef Sawyer::SharedPointer<ConcolicExecutor> ConcolicExecutorPtr;

class TestSuite;
typedef Sawyer::SharedPointer<TestSuite> TestSuitePtr;

class Database;
typedef Sawyer::SharedPointer<Database> DatabasePtr;

class ExecutionManager;
typedef Sawyer::SharedPointer<ExecutionManager> ExecutionManagerPtr;

class LinuxExitStatus;
typedef Sawyer::SharedPointer<LinuxExitStatus> LinuxExitStatusPtr;

class ConcreteExecutorResult;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Database
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** ID class for database objects. */
template <class Tag>
class ObjectId: public Sawyer::Optional<size_t> {
public:
    typedef size_t                  Value;         /**< Type of ID. */
    typedef Sawyer::Optional<Value> Super;         /**< Supertype of this class. */
    typedef Tag                     Object;        /**< Type of object to which this ID refers. */

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

typedef ObjectId<TestSuite> TestSuiteId;                /**< Database ID for test suite objects. */
typedef ObjectId<Specimen> SpecimenId;                  /**< Database ID for specimen objects. */
typedef ObjectId<TestCase> TestCaseId;                  /**< Database ID for test case objects. */
typedef ObjectId<SystemCall> SystemCallId;              /**< Database ID for system call objects. */

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
struct ObjectTraits<SystemCall> {
    using Id = SystemCallId;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
