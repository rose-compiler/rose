#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

TestSuite::Ptr
TestSuite::instance(const std::string& tsname) {
  Ptr res(new TestSuite);

  res->name(tsname);
  return res;
}

std::string
TestSuite::name() const {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return name_;
}

void
TestSuite::name(const std::string& tsname) {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  name_ = tsname;
}

std::string
TestSuite::printableName(const Database::Ptr &db) {
    std::string retval = "testsuite";                   // no white space
    if (db) {
        if (TestSuiteId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    if (!name().empty())
        retval += " \"" + StringUtility::cEscape(name()) + "\"";
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
