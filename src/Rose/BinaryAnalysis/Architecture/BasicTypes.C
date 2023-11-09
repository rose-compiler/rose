#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/Diagnostics.h>

#include <Sawyer/Synchronization.h>
#include <set>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Sawyer::Message::Facility mlog;

// The following variables are protected by registryMutex
static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<Base::Ptr> registry;
static boost::once_flag initFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard loc(registryMutex);
    // FIXME[Robb Matzke 2023-11-09]: none registered yet
    //   registry.push_back(x86::instance());
    //   registry.push_back(amd64::instance());
    //   etc.
}

// Initialize the registry with ROSE's built-in architecture definitions.
static void
initRegistry() {
    boost::call_once(&initRegistryHelper, initFlag);
}

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Architecture");
        mlog.comment("defining hardware architectures");
    }
}

void
registerDefinition(const Base::Ptr &arch) {
    ASSERT_not_null(arch);
    initRegistry();

    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(arch);
}

bool
deregisterDefinition(const Base::Ptr &arch) {
    if (arch) {
        initRegistry();
        SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
        for (size_t i = registry.size(); i > 0; --i) {
            if (registry[i-1] == arch) {
                registry.erase(registry.begin() + (i-1));
                return true;
            }
        }
    }
    return false;
}

std::vector<Base::Ptr>                                  // intentionally not a const ref
registeredDefinitions() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    return registry;                                    // must be copied
}

std::set<std::string>
registeredNames() {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    std::set<std::string> names;
    for (const Base::Ptr &arch: registry)
        names.insert(arch->name());
    return names;
}

Base::Ptr
findByName(const std::string &name) {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        ASSERT_not_null(*iter);
        if ((*iter)->name() == name)
            return *iter;
    }
    return {};
}

const std::string&
name(const Base::Ptr &arch) {
    static const std::string empty;
    return arch ? arch->name() : empty;
}

} // namespace
} // namespace
} // namespace

#endif
