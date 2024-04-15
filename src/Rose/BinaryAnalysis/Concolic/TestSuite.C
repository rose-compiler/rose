#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

TestSuite::TestSuite() {}

TestSuite::~TestSuite() {}

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

std::string
TestSuite::timestamp() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return timestamp_;
}

void
TestSuite::timestamp(const std::string &s) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    timestamp_ = s;
}

} // namespace
} // namespace
} // namespace

#endif
