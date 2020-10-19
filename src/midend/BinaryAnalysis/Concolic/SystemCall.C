#include <sage3basic.h>
#include <Concolic/SystemCall.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Concolic/Database.h>
#include <Concolic/TestCase.h>

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

TestCase::Ptr
SystemCall::testCase() {
    ASSERT_not_implemented("[Robb Matzke 2020-10-14]");
}

void
SystemCall::testCase(TestCaseId tcid) {
    ASSERT_not_implemented("[Robb Matzke 2020-10-14]");
}

void
SystemCall::testCase(const TestCase::Ptr &tc) {
    ASSERT_not_implemented("[Robb Matzke 2020-10-14]");
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
