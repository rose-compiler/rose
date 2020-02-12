#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

TestCase::Ptr
TestCase::instance(const Specimen::Ptr& specimen) {
  Ptr res(new TestCase);

  res->specimen(specimen);
  return res;
}

std::string
TestCase::name() const {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return name_;
}

void
TestCase::name(const std::string& tcname) {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  name_ = tcname;
}

std::string
TestCase::printableName(const Database::Ptr &db) {
    std::string retval = "testcase";                    // no white space
    if (db) {
        if (TestCaseId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    if (!name().empty())
        retval += " \"" + StringUtility::cEscape(name()) + "\"";
    return retval;
}

Specimen::Ptr
TestCase::specimen() const {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return specimen_;
}

void
TestCase::specimen(const Specimen::Ptr& tcspecimen) {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  specimen_ = tcspecimen;
}

std::vector<EnvValue>
TestCase::env() const {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return env_;
}

void
TestCase::env(std::vector<EnvValue> envvars) {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  envvars.swap(env_);
}

std::vector<std::string>
TestCase::args() const {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return args_;
}

void
TestCase::args(std::vector<std::string> cmdlineargs)
{
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  cmdlineargs.swap(args_);
}

bool 
TestCase::hasConcolicTest() const { 
    //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return concolicResult_;
}

bool 
TestCase::hasConcreteTest() const
{
  return concreteRank();
}

Sawyer::Optional<double> 
TestCase::concreteRank() const {
    //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return concreteRank_;
}

void
TestCase::concreteRank(Sawyer::Optional<double> val) {
    //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    concreteRank_ = val;
}

} // namespace
} // namespace
} // namespace

#endif
