#ifndef ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

// Subdirectory/subnamespace #include's are at the *end* of this file.
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/StringUtility/Escape.h>

#include <Rose/Exception.h>
#include <rose_strtoull.h>

#include <Sawyer/Callbacks.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedPointer.h>

#include <boost/shared_ptr.hpp>
#include <memory>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Flags and enums
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Update { NO, YES };

/** Whether to show execution events. */
enum class ShowEvents {
    NONE,                                               /**< Do not show events. */
    INPUT,                                              /**< Show only input events. */
    ALL                                                 /**< Show all events. */
};

enum class ShowAssertions { NO, YES };

/** The different kinds of program inputs. */
enum class InputType {
    NONE,                                               /**< Not an input type. */
    ARGC,                                               /**< Number of program arguments. */
    ARGV,                                               /**< Variable is (part of) a program argument. */
    ENVP,                                               /**< Variable is (part of) a program environment. */
    SYSCALL_RET,                                        /**< Variable is return value of system call. */
    SHMEM_READ                                          /**< Variable is the result of reading shared memory. */
};

/** Direction of access. */
enum class IoDirection {
    READ,                                               /**< Data is being read from memory. */
    WRITE                                               /**< Data is being written to memory. */
};

/** When something happens.
 *
 *  This is usually used to describe when an execution event happens relative to a particular instruction. */
enum class When {
    PRE,                                                /**< Event is replayed before the corresponding instruction. */
    POST                                                /**< Event is replayed after the corresponding instruction. */
};

/** Concolic execution phase. */
enum class ConcolicPhase {
    REPLAY,                                             /**< Concrete execution to initialize concolic state. */
    EMULATION,                                          /**< Concolic execution of an instruction. */
    POST_EMULATION                                      /**< Actions that occur after concolic execution of an instruction. */
};

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

/** Test whether calling process is running in a container.
 *
 *  Returns true if the current process appears to be running in a container such as Docker or Podman. */
bool isRunningInContainer();

/** Print expression or value with additional data on one line.
 *
 *  For example, a one-byte constant would print the numeric value and a character value.
 *
 * @{ */
std::string toString(const SymbolicExpressionPtr&, SymbolicExpression::Formatter&);
std::string toString(const SymbolicExpressionPtr&);
std::string toString(uint64_t value, size_t nBits);
/** @} */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward references
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Architecture;
using ArchitecturePtr = Sawyer::SharedPointer<Architecture>;

class ConcolicExecutor;
using ConcolicExecutorPtr = Sawyer::SharedPointer<ConcolicExecutor>;

class ConcolicExecutorSettings;

class ConcreteExecutor;
using ConcreteExecutorPtr = Sawyer::SharedPointer<ConcreteExecutor>;

class ConcreteResult;
using ConcreteResultPtr = Sawyer::SharedPointer<ConcreteResult>;

class Database;
using DatabasePtr = Sawyer::SharedPointer<Database>;

namespace Emulation {
    class RiscOperators;
    using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;
}

class ExecutionEvent;
using ExecutionEventPtr = Sawyer::SharedPointer<ExecutionEvent>;

class ExecutionLocation;

class ExecutionManager;
using ExecutionManagerPtr = Sawyer::SharedPointer<ExecutionManager>;

class InputVariables;
using InputVariablesPtr = Sawyer::SharedPointer<InputVariables>;

class SharedMemoryCallback;
using SharedMemoryCallbackPtr = Sawyer::SharedPointer<SharedMemoryCallback>;

using SharedMemoryCallbacks = Sawyer::Callbacks<SharedMemoryCallbackPtr>;

class SharedMemoryContext;

class Specimen;
using SpecimenPtr = Sawyer::SharedPointer<Specimen>;

class SyscallCallback;
using SyscallCallbackPtr = std::shared_ptr<SyscallCallback>;

using SyscallCallbacks = Sawyer::Callbacks<SyscallCallbackPtr>;

class SyscallContext;

class TestCase;
using TestCasePtr = Sawyer::SharedPointer<TestCase>;

class TestSuite;
using TestSuitePtr = Sawyer::SharedPointer<TestSuite>;

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

    // Useful for database operations
    const Super& optional() const {
        return *this;
    }
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

// #include's for subdirectories
#include <Rose/BinaryAnalysis/Concolic/Callback/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/BasicTypes.h>

#endif
