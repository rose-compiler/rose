#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>

// Concrete executors pre-registered as factories
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ExitStatusExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/TracingExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingExecutor.h>

#include <boost/thread/once.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<ConcreteExecutor::Ptr> registry;
static boost::once_flag registryInitFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(I386Linux::ExitStatusExecutor::factory());
    registry.push_back(I386Linux::TracingExecutor::factory());
    registry.push_back(M68kSystem::TracingExecutor::factory());
}

static void
initRegistry() {
    boost::call_once(&initRegistryHelper, registryInitFlag);
}

ConcreteExecutor::ConcreteExecutor(const std::string &name)
    : name_(name) {}

ConcreteExecutor::ConcreteExecutor(const Database::Ptr &db)
    : db_(db) {
    ASSERT_not_null(db);
}

ConcreteExecutor::~ConcreteExecutor() {}

void
ConcreteExecutor::registerFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(factory);
}

bool
ConcreteExecutor::deregisterFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        if (*iter == factory) {
            registry.erase(std::next(iter).base());
            return true;
        }
    }
    return false;
}

std::vector<ConcreteExecutor::Ptr>
ConcreteExecutor::registeredFactories() {
    initRegistry();
    std::vector<Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registry.size());
    for (const Ptr &factory: registry)
        retval.push_back(factory);
    return retval;
}

ConcreteExecutor::Ptr
ConcreteExecutor::forge(const Database::Ptr &db, const std::string &name) {
    ASSERT_not_null(db);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
        if ((*factory)->matchFactory(name))
            return (*factory)->instanceFromFactory(db);
    }
    return {};
}

bool
ConcreteExecutor::isFactory() const {
    return !db_;
}

const std::string&
ConcreteExecutor::name() const {
    return name_;
}

void
ConcreteExecutor::name(const std::string &s) {
    name_ = s;
}

Database::Ptr
ConcreteExecutor::database() const {
    return db_;
}

} // namespace
} // namespace
} // namespace
#endif
