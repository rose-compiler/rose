#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

#include <Rose/BinaryAnalysis/Architecture/Amd64.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Architecture/Cil.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium4.h>
#include <Rose/BinaryAnalysis/Architecture/Jvm.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc64.h>
#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>

#include <Sawyer/Synchronization.h>
#include <boost/lexical_cast.hpp>
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
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);

    // Order is important -- lookup searches progress from the end toward the beginning.  Therefore, most specific and/or newest
    // architectures should be closer to the end.

    // Byte code
    registry.push_back(Cil::instance());
    registry.push_back(Jvm::instance());

    // ARM
    registry.push_back(ArmAarch32::instance(ArmAarch32::InstructionSet::T32));
    registry.push_back(ArmAarch32::instance(ArmAarch32::InstructionSet::A32));
    registry.push_back(ArmAarch64::instance());

    // MIPS
    registry.push_back(Mips32::instance(ByteOrder::ORDER_MSB));
    registry.push_back(Mips32::instance(ByteOrder::ORDER_LSB));

    // Motorola
    registry.push_back(Motorola68040::instance());
    registry.push_back(NxpColdfire::instance());

    // PowerPC
    registry.push_back(Powerpc32::instance(ByteOrder::ORDER_MSB));
    registry.push_back(Powerpc32::instance(ByteOrder::ORDER_LSB));
    registry.push_back(Powerpc64::instance(ByteOrder::ORDER_MSB));
    registry.push_back(Powerpc64::instance(ByteOrder::ORDER_LSB));

    // x86
    registry.push_back(Intel8086::instance());
    registry.push_back(Intel8088::instance());
    registry.push_back(Intel80286::instance());
    registry.push_back(IntelI386::instance());
    registry.push_back(IntelI486::instance());
    registry.push_back(IntelPentium::instance());
    registry.push_back(IntelPentiumii::instance());
    registry.push_back(IntelPentiumiii::instance());
    registry.push_back(IntelPentium4::instance());
    registry.push_back(Amd64::instance());
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
    initRegistry();
    std::vector<Base::Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registry.size());
    for (size_t i = registry.size(); i > 0; --i)
        retval.push_back(registry[i-1]);
    return retval;
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

Sawyer::Result<Base::Ptr, NotFound>
findByName(const std::string &name) {
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        ASSERT_not_null(*iter);
        if ((*iter)->matchesName(name))
            return Sawyer::makeOk(*iter);
    }
    return Sawyer::makeError(NotFound("architecture \"" + StringUtility::cEscape(name) + "\" not found"));
}

Sawyer::Result<Base::Ptr, NotFound>
findByHeader(SgAsmGenericHeader *header) {
    ASSERT_not_null(header);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        ASSERT_not_null(*iter);
        if ((*iter)->matchesHeader(header))
            return Sawyer::makeOk(*iter);
    }
    return Sawyer::makeError(NotFound("no architecture found to handle " + std::string(header->formatName()) + " file header"));
}

std::pair<Base::Ptr, size_t>
findBestByInterpretation(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();

    Base::Ptr best;                                     // best matching architecture
    size_t bestN = 0;                                   // number of matches for best architecture

    for (const Base::Ptr &arch: registeredDefinitions()) {
        // How many headers does `arch` match?
        size_t nMatched = 0;
        for (SgAsmGenericHeader *header: headers) {
            if (arch->matchesHeader(header))
                ++nMatched;
        }

        // Return or remember
        if (nMatched == headers.size()) {
            return {arch, nMatched};
        } else if (nMatched > bestN) {
            best = arch;
            bestN = nMatched;
        }
    }

    return std::make_pair(best, bestN);
}

Sawyer::Result<Base::Ptr, NotFound>
findByInterpretation(SgAsmInterpretation *interp) {
    ASSERT_not_null(interp);
    const size_t nHeaders = interp->get_headers()->get_headers().size();
    auto best = findBestByInterpretation(interp);
    if (best.second == nHeaders) {
        ASSERT_not_null(best.first);
        return Sawyer::makeOk(best.first);
    } else if (best.second > 0) {
        return Sawyer::makeError(NotFound("architecture \"" + StringUtility::cEscape(best.first->name()) + "\" matches " +
                                          boost::lexical_cast<std::string>(best.second) + " of " +
                                          StringUtility::plural(nHeaders, "headers")));
    } else {
        return Sawyer::makeError(NotFound("no architecture matches any of the file headers"));
    }
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
