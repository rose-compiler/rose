#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

SystemCall::SystemCall()
    : callNumber_(INVALID_INDEX), functionId_(INVALID_INDEX), callSite_(-1), returnValue_(0) {}

SystemCall::~SystemCall() {}

// class method
SystemCall::Ptr
SystemCall::instance() {
    return Ptr(new SystemCall);
}

// class method
SystemCall::Ptr
SystemCall::instance(const SystemCall::Ptr &orig) {
    SystemCall::Ptr retval = instance();
    retval->callSequenceNumber(orig->callSequenceNumber());
    retval->functionId(orig->functionId());
    retval->callSite(orig->callSite());
    retval->returnValue(orig->returnValue());
    return retval;
}

TestCase::Ptr
SystemCall::testCase() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return testCase_;
}

void
SystemCall::testCase(const TestCase::Ptr &tc) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    testCase_ = tc;
}

std::string
SystemCall::timestamp() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return timestamp_;
}

void
SystemCall::timestamp(const std::string &ts) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    timestamp_ = ts;
}

size_t
SystemCall::callSequenceNumber() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return callNumber_;
}

void
SystemCall::callSequenceNumber(size_t n) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    callNumber_ = n;
}

size_t
SystemCall::functionId() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return functionId_;
}

void
SystemCall::functionId(size_t n) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    functionId_ = n;
}

rose_addr_t
SystemCall::callSite() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return callSite_;
}

void
SystemCall::callSite(rose_addr_t va) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    callSite_ = va;
}

int
SystemCall::returnValue() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return returnValue_;
}

void
SystemCall::returnValue(int retval) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    returnValue_ = retval;
}

std::string
SystemCall::printableName(const Database::Ptr &db) {
    std::string retval = "syscall";
    if (db) {
        if (SystemCallId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
