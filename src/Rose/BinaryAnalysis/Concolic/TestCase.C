#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

TestCase::TestCase()
    : concreteIsInteresting_(false) {}

TestCase::~TestCase() {}

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

TestCaseId
TestCase::parent() const {
    return parent_;
}

void
TestCase::parent(TestCaseId p) {
    parent_ = p;
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

void
TestCase::toYaml(std::ostream &out, const Database::Ptr &db, std::string prefix,
                 ShowEvents showEvents, ShowAssertions showAssertions) {
    ASSERT_not_null(db);
    const TestCaseId tcid = db->id(sharedFromThis(), Update::NO);
    ASSERT_require(tcid);

    out <<prefix <<"testcase: " <<*tcid <<"\n";
    prefix = std::string(prefix.size(), ' ');

    if (!name().empty())
        out <<prefix <<"name: " <<StringUtility::yamlEscape(name()) <<"\n";

    out <<prefix <<"created:  " <<timestamp() <<"\n";
    out <<prefix <<"specimen: " <<*db->id(specimen(), Update::NO) <<" # " <<StringUtility::yamlEscape(specimen()->name()) <<"\n";

    //-------- Inputs ---------
    out <<prefix <<"inputs:\n";

    out <<prefix <<"  command-line:\n";
    for (auto arg: args())
        out <<prefix <<"    - " <<StringUtility::yamlEscape(arg) <<"\n";

    if (!env().empty()) {
        out <<prefix <<"  environment:\n";
        for (const EnvValue &nameValue: env()) {
            out <<prefix <<"    - " <<StringUtility::yamlEscape(nameValue.first)
                <<": " <<StringUtility::yamlEscape(nameValue.second) <<"\n";
        }
    }

    if (TestCaseId p = parent()) {
        out <<prefix <<"  parent: " <<*p <<" # test case ID\n";
    } else {
        out <<prefix <<"  parent: none\n";
    }

    switch (showEvents) {
        case ShowEvents::NONE:
            out <<prefix <<"  execution-events: " <<db->executionEvents(tcid).size() <<"\n";
            break;

        case ShowEvents::INPUT: {
            std::vector<ExecutionEventId> events = db->executionEvents(tcid);
            if (!events.empty()) {
                out <<prefix <<"  execution-events:\n";
                for (ExecutionEventId eeid: events) {
                    ExecutionEvent::Ptr ee = db->object(eeid);
                    if (ee->inputType() != InputType::NONE)
                        ee->toYaml(out, db, prefix + "    - ");
                }
            }
            break;
        }

        case ShowEvents::ALL: {
            std::vector<ExecutionEventId> events = db->executionEvents(tcid);
            if (!events.empty()) {
                out <<prefix <<"  execution-events:\n";
                for (ExecutionEventId eeid: events) {
                    ExecutionEvent::Ptr ee = db->object(eeid);
                    ee->toYaml(out, db, prefix + "    - ");
                }
            }
            break;
        }
    }

    if (ShowAssertions::YES == showAssertions) {
        out <<prefix <<"  assertions:\n";
        for (SymbolicExpression::Ptr assertion: assertions())
            out <<prefix <<"    - " <<*assertion <<"\n";
    } else {
        out <<prefix <<"  assertions: " <<assertions().size() <<"\n";
    }

    //-------- Concrete results ---------
    out <<prefix <<"concrete-result:\n";
    if (hasConcreteTest()) {
        out <<prefix <<"  status:      ran\n"
            <<prefix <<"  rank:        " <<*concreteRank() <<"\n"
            <<prefix <<"  interesting: " <<(concreteIsInteresting() ? "true" : "false") <<"\n";
    } else {
        out <<prefix <<"  status:      not run yet\n";
    }

    //-------- Concolic results ---------
    out <<prefix <<"concolic-result:\n";
    if (hasConcolicTest()) {
        out <<prefix <<"  status:      ran\n";
    } else {
        out <<prefix <<"  status:      not run yet\n";
    }
}

std::string
TestCase::timestamp() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return timestamp_;
}

void
TestCase::timestamp(const std::string &s) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    timestamp_ = s;
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

bool
TestCase::concreteIsInteresting() const {
    return concreteIsInteresting_;
}

void
TestCase::concreteIsInteresting(bool b) {
    concreteIsInteresting_ = b;
}

} // namespace
} // namespace
} // namespace

#endif
