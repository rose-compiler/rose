#include <sage3basic.h>
#include <BinaryConcolic.h>

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

} // namespace
} // namespace
} // namespace
